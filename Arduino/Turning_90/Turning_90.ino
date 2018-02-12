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



Adafruit_SSD1306 display(OLED_RESET);
PololuWheelEncoders enc;
DualVNH5019MotorShield md;

void setup() {
  
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  md.init();
  enc.init( e1a, e1b, e2a, e2b );

}

void loop()
{
  turn(1, 90);

  while(true);
}

void turn(int dir, int turnDegree)
{
    //1 is right, -1 is left
  
    double cir = 54.03539364174444; //circumfrence of circle drawn when turning in cm, current diameter used is 17.1
    double cmToCounts = 119.305728441; //cm to counts for wheel
    double amount = abs(cir * (turnDegree/360.0) * cmToCounts);

    encoders_get_counts_and_reset_m1();
    encoders_get_counts_and_reset_m2();   
  
    while(abs(encoders_get_counts_m1()) < amount - 500)
    {
      Serial.println(encoders_get_counts_m1());
      md.setSpeeds(-260 * dir, 300 * dir);
    }

    //Turn Slower at the Last Bit
    while(abs(encoders_get_counts_m1()) < amount - 100)
    {
      md.setSpeeds(-80 * dir, 125 * dir);
    }
    md.setBrakes(400,400);
    md.setSpeeds(0,0);  
    delay(100);

    //If overshot turn back
    md.setBrakes(0,0);
    while(abs(encoders_get_counts_m1()) > amount)
    {
       md.setSpeeds(80 * dir, -125 * dir);
    }
    md.setBrakes(400,400);
    md.setSpeeds(0,0);
    delay(100);

    //If under turn, turn finish
    md.setBrakes(0,400);
    while(abs(encoders_get_counts_m1()) < amount)
    {
       md.setSpeeds(-80 * dir, 0 * dir);
    }
    md.setBrakes(400,400);
    md.setSpeeds(0,0);
    delay(100);

    md.setBrakes(400,0);
    while(abs(encoders_get_counts_m2()) < amount)
    {
      md.setSpeeds(0 * dir,80 * dir);
    }
    md.setBrakes(400,400);
    md.setSpeeds(0,0);
    delay(100);


    //If over turn, turn finish
    md.setBrakes(0,400);
    while(abs(encoders_get_counts_m1()) > amount)
    {
       md.setSpeeds(80 * dir, 0 * dir);
    }
    md.setBrakes(400,400);
    md.setSpeeds(0,0);
    delay(100);

    md.setBrakes(400,0);
    while(abs(encoders_get_counts_m2()) > amount)
    {
      md.setSpeeds(0 * dir,-80 * dir);
    }
    md.setBrakes(400,400);
    md.setSpeeds(0,0);
    delay(100);
    
    display.clearDisplay();
    display.setTextSize(0.1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println(amount);
    display.println(encoders_get_counts_m1());
    display.println(encoders_get_counts_m2());
    display.display();
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
