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
  
## Hardware.
1.	Raspberry Pi 3B+ -- Used with OpenCv to process images to detect faces and communicate with the Arduino Mega 2560 Rev3.
a.	Camera Module 2 – Used to capture images to be used with OpenCv
2.	Arduino Mega 2560 Rev3 – Used to control the motor controllers and all external devices.
3.	Lin Engineering 4118 Series Stepper Motors x3 – Used to control the turning, chambering, and trigger of the Nerf gun.
4.	Stepper Motor Drivers x3 – Used to control the stepper motors.
5.	7-Segment Display
a.	1 Digit 7-Segment Display – Used to display the remaining ammo count.
b.	74HC595 IC – Used to convert the input from the Arduino Mega into binary values to display on the 7-Segment Display.
6.	HC-SR501 PIR Sensor – Used to detect motion in order to wake the Nerf gun and have it begin scanning for targets.
7.	MAX7219 LED Dot Matrix Module – Used to display different smiley faces depending on which stage of shooting the Nerf gun is in.
8.	LED Tactile Button
a.	Green: Used to tell the Arduino that the Nerf Gun has been reloaded.
b.	Red: Used as a safety that stops all motor functions without removing power from the entire circuit.
9.	Toggle Switch – Used to cut all battery power from the circuit.
10.	5v Voltage Regulator – Used to regulate battery power from 7.4v to 5v in order to power the Raspberry Pi which powers the Arduino.
11.	7.4v 4000 mAh Li-PO Battery – Used to power the entire circuit.

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

## Software and Logic Flow  
An Arduino Mega and a Raspberry Pi were both used in this implementation. They communicate with each other via USB Serial, using simple write/read commands.   
  
The Raspberry Pi has one main task: detect faces. It accomplishes this through the use of a python program built using the opencv library and haar face cascades. A dnn (deep neural net) module was also tested here because it is much more reliable, however it took up far too much processing power and could only process 1 frame per second. The haar cascade method was much faster, allowing the robot to function at an appropriate speed. I also decreased the image window to 200x200 pixels, which gave a significant gain in frames processed per second. As it is, face detection is very computationally-intensive and the Pi is using all of its processing power when active.   
	
When a face is detected (or multiple faces,) the Pi detects the horizontal distance from the center of the camera and chooses the nearest face. Since the robot has no vertical movement, it shoots underneath of whichever face it picks (generally in the thoracic region.) Thus the vertical distance is irrelevant. It uses simple subtraction to determine if the face is to the left or the right, and it has a threshold for the “center” position.     
	
The Pi and Arduino communicate by sending character bytes back and forth. When the pi determines a face is in the center, it sends a character to the Arduino which interprets the command to start pulling the trigger, and then it immediately chambers the gun for the next pass. If a face is on either side, it sends different characters, and the Arduino understands it needs to rotate the robot until another command is sent to stop it.  

The Arduino is in command of all the mechanical systems. It sends signals to the motor drivers, which drive the stepper motors that operate the gun and rotate the robot. It also has within its function loops the commands to activate the LED arrays, the buzzer, and the buttons. It also sends a signal back to the Pi to verify it is ready to operate at the beginning of operation, and every time after reloading.  

