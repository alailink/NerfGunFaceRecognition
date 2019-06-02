#I’m using the opencv and imutils library, which allows multi-threading of the analysis processes
from imutils.video.pivideostream import PiVideoStream
from picamera.array import PiRGBArray
from picamera import PiCamera
import imutils
import time
import serial
import cv2

#This sets up Serial communication with the Arduino
ser = serial.Serial('/dev/ttyACM0',9600)

#This sets up the face cascades (one of which is unused) and the image resolution to process
cascadePathFront = "/home/pi/Desktop/NerfGun/haarcascade_frontalface_default.xml"
cascadePathProfile = "/home/pi/Desktop/NerfGun/haarcascade_profileface.xml"
faceCascade = cv2.CascadeClassifier(cascadePathFront);
profileCascade = cv2.CascadeClassifier(cascadePathProfile);
font = cv2.FONT_HERSHEY_SIMPLEX
widthImage=200
positionHolder=0;

#from the imutils library. The pi can read in raw image matrices and also process them at the same time.
vs = PiVideoStream().start()
time.sleep(2.0)
centerImage = widthImage/2 #x coordinate of center of image

#Waiting for the PIR sensor to activate before doing anything.
loop_break='a'
while loop_break!='s':
    loop_break=ser.read(1)


#main loop of the program. It continuously scans for faces here, and sends instructions as appropriate to the #Arduino.
while(1): 
    img =vs.read()
#resizes it here in order to decrease processing time. A lot of the other options here are just to get the image right #side up and gray.
    img = imutils.resize(img, width=widthImage)
    img = imutils.rotate(img, angle=90)
    #img = cv2.flip(img, -1) # Flip vertically
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    #Used within the loops to be used for logic later.
    distance=widthImage;
    xf=0
    yf=0
    wf=0
    hf=0
    
    #I am using two separate Haar cascades to get both the front and side of the face.
    faces = faceCascade.detectMultiScale( 
        gray,
        scaleFactor = 1.2,
        minNeighbors = 4,
        minSize = (int(20), int(20)),
       )

#The other cascade works well enough and removing this increases the speed.
#    profiles = profileCascade.detectMultiScale(
#        gray,
#         scaleFactor = 1.2,
#         minNeighbors = 4,
#         minSize = (int(20), int(20)),
#        )

    for(x,y,w,h) in faces:
        cv2.rectangle(img, (x,y), (x+w,y+h), (255,0,0), 2)
        distance_temp=centerImage-(x+w/2)
        if abs(distance_temp)<abs(distance):
            distance=distance_temp
            xf=x
            yf=y
            wf=w
            hf=h    

#    for(x,y,w,h) in profiles:
#       cv2.rectangle(img, (x,y), (x+w,y+h), (0,255,0), 2)
#        distance_temp=centerImage-(x+w/2)
#        if abs(distance_temp)<abs(distance):
#            distance=distance_temp
#            xf=x
#            yf=y
#            wf=w
#            hf=h
            
    #using only the x-distance from the center will allow me to position the turret in
    #optimal position for shooting. 

    cv2.rectangle(img, (xf,yf), (xf+wf,yf+hf), (0,0,255), 2)
    loop_break='a'
    
    if distance<widthImage:
        if abs(distance)<25: #20 seemed to be an accurate value that will also allow the
                            #program to operate without infinitely correcting itself
            cv2.putText(img, "**Bang**", ((centerImage-75),30), font, 1, (0,0,255), 2)
            ser.write(b't')
            positionHolder=0;
            while loop_break!='h':
                loop_break=ser.read(1)
                
            
        elif distance>0:
            cv2.putText(img, "<--", ((centerImage-75),30), font, 1, (0,255,0), 2)
            if positionHolder!=1:
                ser.write(b'l')
                positionHolder=1;
            
        else:
            cv2.putText(img, "-->", (centerImage,30), font, 1, (0,255,0), 2)
            if positionHolder!=2:
                ser.write(b'r')
                positionHolder=2;
    
    cv2.imshow('camera',img)
    xf = 0
    yf = 0
    wf = 0
    hf = 0    
    k = cv2.waitKey(10) & 0xff # Press 'ESC' for exiting video
    if k == 27:
        break

# Do a bit of cleanup
print("\n [INFO] Exiting Program and cleanup stuff")
cv2.destroyAllWindows()
vs.stop()
