#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define frontRightIR  A1    //Front right PS2
#define frontLeftIR  A3    //Front left PS4
#define left A0  // Left PS1
#define right A2 // Right PS3
int sensorValue = 0;  // variable to store the value coming from the sensor
int distanceFR = 0;
int distanceFL = 0;
int distanceR = 0;
int distanceL = 0;

double LRaw = 0;
double LVoltage = 0;
double RRaw = 0;
double RVoltage = 0;

//OLED def
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  Serial.begin(115200);
  Serial.print("HI: ");
}

void loop() {
  LRaw = analogRead(left);
  LVoltage = LRaw*(5.0 / 1023.0) ;
  /* 
  Serial.print("LRaw: ");
  Serial.print(LRaw);
  Serial.print("  LVoltage: ");
  Serial.println(LVoltage);
  */
  distanceFR = (6787/analogRead(frontRightIR) - 3) - 4;
  distanceFL = (6787/analogRead(frontLeftIR) - 3) - 4;
  //distanceR = (6787/analogRead(right) - 3) ;
  //distanceL = (6787/analogRead(left) - 3) ;

  
  //distanceR = 60.374 * pow( ( analogRead(right)*(5.0 / 1023.0) ) , -1.16);
  distanceL = (60.374 * pow( ( analogRead(left)*(5.0 / 1023.0) ) , -1.16)) + 5;

  
  Serial.print("Front Left: ");
  Serial.println(distanceFL);
  Serial.print("Front Right: ");
  Serial.println(distanceFR);
  
  Serial.print("Left: ");
  Serial.println(distanceL);
  Serial.print("Right: ");
  Serial.println(distanceR);
 
  Serial.println();
/*
  // Setup OLED display
  display.clearDisplay();
  display.setTextSize(0.08);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("FL, FR, L, R: ");
  display.print(distanceFL);
  display.print(", ");
  display.print(distanceFR);
  display.print(", ");
  display.print(distanceL);
  display.print(", ");
  display.println(distanceR);
  display.display();
  
  */
  delay(200);
}
