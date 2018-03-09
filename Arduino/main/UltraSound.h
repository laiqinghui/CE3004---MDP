// # Connection:
// #       Pin 1 VCC (URM V3.2) -> VCC (Arduino)
// #       Pin 2 GND (URM V3.2) -> GND (Arduino)
// #       Pin 4 PWM (URM V3.2) -> Pin 3 (Arduino)
// #       Pin 6 COMP/TRIG (URM V3.2) -> Pin 5 (Arduino)
// #
int URPWM = 5; // PWM Output 0－25000US，Every 50US represent 1cm E1B
int URTRIG=2; // PWM trigger pin
 
unsigned int Distance=0;
uint8_t EnPwmCmd[4]={0x44,0x02,0xbb,0x01};    // distance measure command through PWM
uint8_t DMcmd[4] = {0x22, 0x00, 0x00, 0x22}; //distance measure command

void PWM_Mode_Setup()
{ 
  pinMode(URTRIG,OUTPUT);                     // A low pull on pin COMP/TRIG
  digitalWrite(URTRIG,HIGH);                  // Set to HIGH
  
  pinMode(URPWM, INPUT);                      // Sending Enable PWM mode command
  
  for(int i=0;i<4;i++)
  {
      Serial.write(EnPwmCmd[i]);
  } 
}

void Serial_Mode_Setup()
{ 
  for(int i=0;i<4;i++)
  {
    Serial.write(DMcmd[i]);
  } 
}

char getUltraSoundDistance(){
  
  char USValue = -1;

  //delay(40); //delay for 75 ms
  unsigned long timer = millis();
  while(millis() - timer < 30)
  {
    if(Serial.available()>0)
    {
      int header=Serial.read(); //0x22
      int highbyte=Serial.read();
      int lowbyte=Serial.read();
      int sum=Serial.read();//sum

      if(header == 0x22){
        if(highbyte==255)
        {
          USValue=-1;  //if highbyte =255 , the reading is invalid.
        }
        else
        {
          USValue = highbyte*255+lowbyte;
        }

      }
      else{
        while(Serial.available())  byte bufferClear = Serial.read();
        break;
      }
    }
  }
  return USValue;
}   
 
unsigned long getPWMReading()
{
	OCR1A = 0;
	digitalWrite(URTRIG, HIGH); 
	Serial.println("Set");
	Serial.println(PIND, BIN);
    digitalWrite(URTRIG, LOW);
	Serial.println(PIND, BIN);
    digitalWrite(URTRIG, HIGH);               // reading Pin PWM will output pulses
     
    unsigned long value = pulseIn(URPWM,LOW);
	return value;
}
 
 //Use motor 1 input A, digital pin 2 as trigger for sensor reading
 //Use motor 1 E1B to read sensor output which is digital pin 5
unsigned int getUltraSound2Reading()
{
  unsigned long DistanceMeasured = 0;
    while(DistanceMeasured == 0 || DistanceMeasured>=10200)
    {
      DistanceMeasured = getPWMReading();
    }
	Serial.println(PIND, BIN);
    return DistanceMeasured/50;           // every 50us low level stands for 1cm
}