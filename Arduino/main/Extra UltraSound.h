// # Connection:
// #       Pin 1 VCC (URM V3.2) -> VCC (Arduino)
// #       Pin 2 GND (URM V3.2) -> GND (Arduino)
// #       Pin 4 PWM (URM V3.2) -> Pin 3 (Arduino)
// #       Pin 6 COMP/TRIG (URM V3.2) -> Pin 5 (Arduino)
// #
int URPWM = 5; // PWM Output 0－25000US，Every 50US represent 1cm
int URTRIG=2; // PWM trigger pin
 
unsigned int Distance=0;
uint8_t EnPwmCmd[4]={0x44,0x02,0xbb,0x01};    // distance measure command
 
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
unsigned long getUltraSound2()
{
		OCR1A = 0;
		digitalWrite(URTRIG,HIGH);  
	     digitalWrite(URTRIG, LOW);
    digitalWrite(URTRIG, HIGH);               // reading Pin PWM will output pulses
     
    return pulseIn(URPWM,LOW);
}
 
 //Use motor 1 input A, digital pin 2 as trigger for sensor reading
 //Use motor 1 E1B to read sensor output which is digital pin 5
int getUltraSound2Reading()
{                              
	// a low pull on pin COMP/TRIG  triggering a sensor reading
/*
	PORTD = PORTD & B11111011; //Set to low
	PORTD = PORTD | B10000100; //Set to high
	
           
	// reading Pin PWM will output pulses 
	*/
    unsigned long DistanceMeasured=0;
    
	while(DistanceMeasured == 0 || DistanceMeasured>=10200)
	{
		DistanceMeasured = getUltraSound2();
	}
      return DistanceMeasured/50;           // every 50us low level stands for 1cm

}