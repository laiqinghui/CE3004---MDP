#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DualVNH5019MotorShield.h"
#include "PololuWheelEncoders.h"

//OLED def
#define OLED_RESET 4

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//IR Sensor def
#define ps1 A0

//UltraSound def
int val = 0;
int USValue = 0;
int timecount = 0; // Echo counter
boolean flag=true;
uint8_t DMcmd[4] = { 
  0x22, 0x00, 0x00, 0x22}; //distance measure command


Adafruit_SSD1306 display(OLED_RESET);
PololuWheelEncoders enc;
DualVNH5019MotorShield md;


void setup() {
  
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  md.init();
  enc.init( e1a, e1b, e2a, e2b );
  
  Serial.print("M1 initial En count: ");
  Serial.println(encoders_get_counts_m1());
  Serial.print("M2 initial En count: ");
  Serial.println(encoders_get_counts_m2());
  

  md.setM1Speed(200);
  md.setM2Speed(-200);
  delay(4000);
  md.setM1Speed(0);
  md.setM2Speed(0);

  Serial.print("M1 new En count: ");
  Serial.println(encoders_get_counts_m1());
  Serial.print("M2 new En count: ");
  Serial.println(encoders_get_counts_m2());

  
  // Setup OLED display
  display.clearDisplay();
  display.setTextSize(0.1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("M1: ");
  display.println(encoders_get_counts_m1());
  display.display();

  delay(200);


}

void loop() {

   display.clearDisplay();
   display.setCursor(0,0);
   display.println("IR: ");
   display.println(analogRead(ps1));
   display.display();
   Serial.print("Ultrasound Distance = ");
   Serial.println(getUltraSoundDistance());
   
   delay(1000);
   

}

int getUltraSoundDistance(){
  
    flag = true;
  //Sending distance measure command :  0x22, 0x00, 0x00, 0x22 ;

  for(int i=0;i<4;i++)
  {
    Serial.write(DMcmd[i]);
  }

  delay(40); //delay for 75 ms
  
    if(Serial.available()>0)
    {
      int header=Serial.read(); //0x22
      int highbyte=Serial.read();
      int lowbyte=Serial.read();
      int sum=Serial.read();//sum

      if(header == 0x22){
        if(highbyte==255)
        {
          USValue=65525;  //if highbyte =255 , the reading is invalid.
        }
        else
        {
          USValue = highbyte*255+lowbyte;
        }

        

        flag=false;
      }
      else{
        while(Serial.available())  byte bufferClear = Serial.read();
        
      }
    }
  
  delay(20);
  return USValue;
  
  }

void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}
