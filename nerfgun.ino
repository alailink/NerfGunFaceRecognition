//Pre-set-up
byte ammoCount=6;
char temp;
char serial_input;

bool b_trigger=false;
bool b_cock=false;
bool b_left=false;
bool b_right=false;

//The stepper motors gave me the most trouble of //the entire project.
#include <Stepper.h>
  Stepper myStepper(200, 5, 6, 7, 8);
  Stepper travelMotor(200, 9, 10, 11, 12);

//Caitlyn's LED array
#include "LedControl.h"
int DIN = 36;
int CS =  38;
int CLK = 40;

LedControl lc=LedControl(DIN,CLK,CS,0);

byte dir=2;
byte stp=3;
byte en=4;
byte PIR=34;

//Kyle's 7-segment additions
byte seven_seg_digits[8] = { B11111100,  // = 0
                             B01100000,  // = 1
                             B11011010,  // = 2
                             B11110010,  // = 3
                             B01100110,  // = 4
                             B10110110,  // = 5
                             B10111110,  // = 6
                             B00000000   // = OFF
                           };
int dataPin = 22;    //2A to 13C
int latchPin = 24;   //3A to 10C
int clockPin = 26;   //4A to 9C
int buzzerPin = 28;  //5A to +Buzzer
int reloadPin = 30;  //9A to B1 Ground

//define the LED array faces
     byte smile[8]=   {0x3C,0x42,0xA5,0x81,0xA5,0x99,0x42,0x3C};
    byte neutral[8]= {0x3C,0x42,0xA5,0x81,0xBD,0x81,0x42,0x3C};
    byte frown[8]=   {0x3C,0x42,0xA5,0x81,0x99,0xA5,0x42,0x3C};

void setup() {

  Serial.begin(9600);       
  pinMode(dir, OUTPUT); //dir
  pinMode(stp, OUTPUT); //step
  pinMode(en,OUTPUT); //enable
  digitalWrite(dir, LOW);
  digitalWrite(stp, LOW);
  digitalWrite(en,HIGH);

  //7-segment and buzzer
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(reloadPin, INPUT);
  //stop all button 32
  //motion sensor 34
  sevenSegWrite(ammoCount);
  
  //Robot stays here until PIR sensor goes off
  //David's PIR sensor
  pinMode(PIR, INPUT);
  int PIR_read=LOW;
  while(PIR_read==LOW){
  PIR_read= digitalRead(PIR);
  Serial.print('s');
  }
  //LED array
   lc.shutdown(0,false);       
//The MAX72XX is in power-saving mode on startup
 lc.setIntensity(0,15);      
// Set the brightness to maximum value
 lc.clearDisplay(0);         // and clear the display

//pre-cock the gun before face-detection starts
  cock();
}


//**MAIN LOOP********//
void loop() {
  
  
  //constantly checking the pi for updates.
  //the pi is the brain. The arduino just executes commands, like a perfect soldier.
  serial_read_and_bool_determination();

  if(ammoCount==0){
    while(1){
      //use an interrupt or poll and #break. Probably will use pin 1.
      //ammoCount=6; 
    }
  }
  if(b_trigger)
    trigger();
  if(b_cock)
    cock();
  if(b_right)
    motorRight();
  if(b_left)
    motorLeft();
 
}
//**END MAIN LOOP********//

//Here are all the serial read commands and most //of the logic that determines the robots entire //entire functionality.
void serial_read_and_bool_determination(){
    if(Serial.available()>0){
      temp=Serial.read();
      if((temp!='\n')&&(temp!='\r')){
        serial_input=temp;
        digitalWrite(9,LOW);
        digitalWrite(10,LOW);
        digitalWrite(11,LOW);
        digitalWrite(12,LOW);
        switch (serial_input){
          case 'c':
            b_cock=true;
            break;
          case 't':
            b_trigger=true;
            b_right=false;
            b_left=false;
            break;
          case 'r':
            b_right=true;
            b_left=false;
            break;
          case 'l':
            b_left=true;
            b_right=false;
            break;
	break;
        }
      }   
    }
}

//the rest of this code is pretty self-explanatory //through the simple fact that I used very //descriptive and relevant variable/function names.

void cock(){
    int delayCock=800;
    int steps=620;
    digitalWrite(dir,HIGH);
    digitalWrite(en,LOW);
    
    for(int i=0;i<steps;i++){
    digitalWrite(stp, HIGH);
    delayMicroseconds(delayCock);          
    digitalWrite(stp, LOW); 
    delayMicroseconds(delayCock);   
    }

    digitalWrite(dir,LOW);
    for(int i=0;i<steps;i++){
    digitalWrite(stp, HIGH);
    delayMicroseconds(delayCock);          
    digitalWrite(stp, LOW); 
    delayMicroseconds(delayCock);   
    }
    digitalWrite(en,HIGH);
    b_cock=false;
    Serial.print('h');
    printByte(smile);
}

void trigger(){
    printByte(frown);
    myStepper.setSpeed(80);
    myStepper.step(270);//(stepsPerRevolution / 100);
    myStepper.step(-270);//(stepsPerRevolution / 100);   
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
    digitalWrite(7,LOW);
    digitalWrite(8,LOW);
    b_trigger=false;
    b_cock=true;
    ammoCount=ammoCount-1;
    sevenSegWrite(ammoCount);
    if(ammoCount==0){
      while(digitalRead(reloadPin)!=HIGH){
      sevenSegWrite(0);
      delay(100);
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      sevenSegWrite(7);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
    }
    ammoCount = 6;
    sevenSegWrite(ammoCount);
    digitalWrite(buzzerPin,LOW);
    }

}

void motorRight(){
    travelMotor.setSpeed(10);
    travelMotor.step(1);

}

void motorLeft(){
    travelMotor.setSpeed(10);
    travelMotor.step(-1);
   
}

void sevenSegWrite(unsigned char digit)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, seven_seg_digits[digit]);
  digitalWrite(latchPin, HIGH);
}

void printByte(byte character [])
{
  int i = 0;
  for(i=0;i<8;i++)
  {
    lc.setRow(0,i,character[i]);
  }
}
