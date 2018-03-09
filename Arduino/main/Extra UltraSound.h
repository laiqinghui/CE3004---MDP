// # Connection:
// #       Pin 1 VCC (URM V3.2) -> VCC (Arduino)
// #       Pin 2 GND (URM V3.2) -> GND (Arduino)
// #       Pin 4 PWM (URM V3.2) -> Pin 3 (Arduino)
// #       Pin 6 COMP/TRIG (URM V3.2) -> Pin 5 (Arduino)
// #
int URPWM = 5; // PWM Output 0－25000US，Every 50US represent 1cm E1B
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

unsigned long getReading()
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
      DistanceMeasured = getReading();
    }
	Serial.println(PIND, BIN);
    return DistanceMeasured/50;           // every 50us low level stands for 1cm
}

void benForward(int rpm, double distance, boolean pidOn) {

  signed long tuneEntryTime = 0;
  signed long tuneExitTime = 0;
  signed long interval = 0;
  double offset = 1;
  if (distance == 9.5)
    offset = 0.95;

  double distanceTicks = offset  * distance * ticksPerCM;//Delibrate trimming
  unsigned long currentTicksM1 = 0;
  unsigned long currentTicksM2 = 0;

  MotorPID M1pid = {260, 0, 0, 0.1};//0.1=>50
  MotorPID M2pid = {300 , 0, 0, 0.115};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

  enableInterrupt( e1a, risingM1, RISING);
  enableInterrupt( e2b, risingM2, RISING);

  //md.setSpeeds(100, 100);
  md.setSpeeds(260, 300);


  Serial.print("Target Ticks: ");
  Serial.println(distanceTicks);

  while (1) {

    //Serial.print(sqWidthToRPM(squareWidth_M1));
    //Serial.print(" ");
    //Serial.println(sqWidthToRPM(squareWidth_M2));

    if (pidOn) {
      tuneEntryTime = micros();
      interval = tuneEntryTime - tuneExitTime;
      if (interval >= 3000) {

        tuneM1(rpm, &M1pid);
        tuneM2(rpm, &M2pid);

        tuneExitTime = micros();
      }
    }
    noInterrupts();
    currentTicksM1 = M1ticks;
    currentTicksM2 = M2ticks;
    interrupts();

    if (currentTicksM2 >= distanceTicks) {
      md.setBrakes(400, 400);
      break;
    }  //end of if


  }//end of while

  //md.setBrakes(400,400);
  disableInterrupt(e1a);
  disableInterrupt(e2b);
  setTicks(0, 0);
  setSqWidth(0, 0);
}