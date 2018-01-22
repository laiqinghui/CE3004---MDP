#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DualVNH5019MotorShield.h"
#include "PololuWheelEncoders.h"
//#include "URMSerial.h"

//OLED def
#define OLED_RESET 4

//Wheel Encoders def
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13

//IR Sensor def
#define ps1 A0

/*
//UltraSound def
#define DISTANCE 1
#define TEMPERATURE 2
#define ERROR 3
#define NOTREADY 4
#define TIMEOUT 5
#define ENABLE 8
*/

Adafruit_SSD1306 display(OLED_RESET);
PololuWheelEncoders enc;
DualVNH5019MotorShield md;


void setup() {
  
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  Serial.println("Dual VNH5019 Motor Shield");
  
  md.init();
  enc.init( e1a, e1b, e2a, e2b );
  
  Serial.print("M1 initial En count: ");
  Serial.println(encoders_get_counts_m1());
  Serial.print("M2 initial En count: ");
  Serial.println(encoders_get_counts_m2());
  

  md.setM1Speed(200);
  md.setM2Speed(-220);
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


}

void loop() {

   display.clearDisplay();
   display.setCursor(0,0);
   display.println("IR: ");
   display.println(analogRead(ps1));
   display.display();
   delay(1000);
   

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
