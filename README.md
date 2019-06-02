# NerfGunFaceRecognition  
Facial detection based on opencv. This code is written for a chamber-type nerf gun with stepper motors.  

A lot of this is based on this awesome guy:  
Adrian Rosebrock, Raspberry Pi Face Recognition, PyImageSearch, https://www.pyimagesearch.com/2018/06/25/raspberry-pi-face-recognition/  
  
Definitely take a look at his tutorials for anything opencv + python.  

![nerf1](https://github.com/alailink/NerfGunFaceRecognition/blob/master/nerf1.jpg)  
![nerf2](https://github.com/alailink/NerfGunFaceRecognition/blob/master/nerf2.jpg)  

The basic design is a nerf gun mounted onto a rotating platform, which continuously rotates until the picamera in the front detects a face. When this happens, a series of stepper motors will chamber the nerf gun and fire the trigger at the target. It will repeat this indefinitely, or up to 6 times (when the gun is empty.) 

The stepper motors are connected to the chambering mechanism and trigger through a pulley and fishing line. The fishing line has a tendency to break though, and it is suggested to use a stronger material such as braided wire, cable, or a belt. The rotation is also accomplished through a stepper motor. 

For the face detection, I just used Adrian's tutorial, but took away the facial recognition functionality and only used the face detection. The difference is subtle, but important. Facial Detection is taking an image and identifying that a face is present, and finding the location of that face. The next step is facial recognition which would be identifying the person that face belonged to. But in this project it was unnecessary.
  
### Stepper Motor + Driver  
This is fairly standard for anyone that's used a stepper motor. But for some reason they never get easier to figure out.   
![stepperdriver](https://github.com/alailink/RF-Coupling-Automation/blob/master/FourWireSystem/stepperdriver.jpg)  
* Note that reset/sleep must be shorted together.
* Step/Dir are digital inputs from the arduino. I also used the enable pin to conserve power when not in use.
* I pulled MS2 high for a smaller step resolution (1/4)  
  
Wire Coding for my 8-wire stepper motor:
* 2B-Green + Black 
* 2A-Striped Green + Black
* 1A-Striped Red + Blue
* 1B-Red + Blue  

