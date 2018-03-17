//------------PID Structs------------
struct MotorPID {
  int prevTuneSpeed;
  signed long currentErr;
  signed long prevErr1;
  float gain;
};

//------------Interrupt declarations------------
volatile int squareWidth_M1 = 0;
volatile signed long prev_time_M1 = 0;
volatile signed long entry_time_M1 = 0;
volatile unsigned long M1ticks = 0;

volatile int squareWidth_M2 = 0;
volatile signed long prev_time_M2 = 0;
volatile signed long entry_time_M2 = 0;
volatile unsigned long M2ticks = 0;

//----------------------------------------------------------------PID----------------------------------------------------------------
//ISR for Motor1(Right) encoder
void risingM1() {
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1ticks++;
}

void risingM1Test(){
  //Check e2b D13
  int M2Status = PINB >> 5;
  entry_time_M1 = micros();
  squareWidth_M1 = entry_time_M1 - prev_time_M1;
  prev_time_M1 = entry_time_M1;
  M1ticks++;
  if(M2Status == 0 && PINB >> 5 == 1)
  {
	  M2ticks++;
  }
}

void risingM2Test() {
  //Check e1a D3
  int M1Status = (PIND >> 3) % 2;	
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = entry_time_M2;
  M2ticks++;
    if(M1Status == 0 && (PIND >> 3) % 2 == 1)
  {
	  M1ticks++;
  }
  
}

void risingM1Ticks() {
  M1ticks++;
}

void risingM2Ticks() {
  M2ticks++;
}

//ISR for Motor2(Left) encoder
void risingM2() {
  entry_time_M2 = micros();
  squareWidth_M2 = entry_time_M2 - prev_time_M2;
  prev_time_M2 = entry_time_M2;
  M2ticks++;
}

void setTicks(int M1, int M2) {
  M1ticks = M1;
  M2ticks = M2;
}

void setSqWidth(int M1, int M2) {
  squareWidth_M1 = M1;
  squareWidth_M2 = M2;
}

signed long sqWidthToRPM(int sqWidth) {

  if (sqWidth <= 0)
    return 0;
  static double sqwavesPerRev = 562.25;
  signed long sqwavesOneS = 1000000 / sqWidth; //1/(sqWidth/1000000)
  signed long sqwavesOneM = sqwavesOneS * 60;
  signed long revPerMin = sqwavesOneM / sqwavesPerRev;

  return revPerMin;

}

void tuneM1(int desiredRPM, MotorPID *M1) {


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M1);

  M1->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M1->prevTuneSpeed + 0.47*M1->currentErr;
  tuneSpeed = M1->prevTuneSpeed + M1->gain * M1->currentErr + (M1->gain / 0.05) * (M1->currentErr - M1->prevErr1);

  OCR1A = tuneSpeed;
  M1->prevTuneSpeed = tuneSpeed;
  M1->prevErr1 = M1->currentErr;

  //Serial.print("M1 tunespeed: ");
  //Serial.println(tuneSpeed);


}

void tuneM2(int desiredRPM, MotorPID *M2) {


  double tuneSpeed = 0;
  double currentRPM = sqWidthToRPM(squareWidth_M2);

  M2->currentErr =  desiredRPM - currentRPM;
  //tuneSpeed = M2->prevTuneSpeed + 0.5*M2->currentErr;
  tuneSpeed = M2->prevTuneSpeed + M2->gain * M2->currentErr + (M2->gain / 0.05) * (M2->currentErr - M2->prevErr1);

  OCR1B = tuneSpeed;
  M2->prevErr1 = M2->currentErr;
  M2->prevTuneSpeed = tuneSpeed;


}

void tuneMotors(int desiredRPM, MotorPID *M1, MotorPID *M2) {

  noInterrupts();
  double currentM1RPM = sqWidthToRPM(squareWidth_M1);
  double currentM2RPM = sqWidthToRPM(squareWidth_M2);
  interrupts();

  int tuneSpeedM1 = 0;
  int tuneSpeedM2 = 0;

  M1->currentErr =  desiredRPM - currentM1RPM;
  tuneSpeedM1 = M1->prevTuneSpeed + M1->gain * M1->currentErr + (M1->gain / 0.05) * (M1->currentErr - M1->prevErr1);
  M2->currentErr =  desiredRPM - currentM2RPM;
  tuneSpeedM2 = M2->prevTuneSpeed + M2->gain * M2->currentErr + (M2->gain / 0.05) * (M2->currentErr - M2->prevErr1);

  noInterrupts();
  OCR1A = tuneSpeedM1;
  OCR1B = tuneSpeedM2;
  interrupts();

  M1->prevTuneSpeed = tuneSpeedM1;
  M1->prevErr1 = M1->currentErr;
  M2->prevTuneSpeed = tuneSpeedM2;
  M2->prevErr1 = M2->currentErr;


}
