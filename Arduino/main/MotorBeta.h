void tuneMotors(int desiredRPM, MotorPID *M1, MotorPID *M2){
  
  noInterrupts();
  double currentM1RPM = sqWidthToRPM(squareWidth_M1);
  double currentM2RPM = sqWidthToRPM(squareWidth_M2);
  interrupts();
  
  int tuneSpeedM1 = 0;
  int tuneSpeedM2 = 0;
  
  M1->currentErr =  desiredRPM - currentM1RPM;
  tuneSpeedM1 = M1->prevTuneSpeed + M1->gain*M1->currentErr + (M1->gain/0.05)*(M1->currentErr - M1->prevErr1);
  M2->currentErr =  desiredRPM - currentM2RPM;
  tuneSpeedM2 = M2->prevTuneSpeed + M2->gain*M2->currentErr + (M2->gain/0.05)*(M2->currentErr - M2->prevErr1);

  md.setSpeeds(tuneSpeedM1, tuneSpeedM2);
  
  M1->prevTuneSpeed = tuneSpeedM1;
  M1->prevErr1 = M1->currentErr;
  M2->prevTuneSpeed = tuneSpeedM2;
  M2->prevErr1 = M2->currentErr;

 
  }

void moveForwardOneGrid(int rpm){

   unsigned long tuneEntryTime = 0;
   unsigned long tuneExitTime = 0;
   unsigned long interval = 0;

   double distanceTicks = 0.95 * 9.5 * ticksPerCM;
   unsigned long currentTicksM1 = 0;
   unsigned long currentTicksM2 = 0;
   
   int m1setSpeed = 100;
   int m2setSpeed = 100;
 
   MotorPID M1pid = {m1setSpeed, 0, 0, 0.1};//0.1=>50
   MotorPID M2pid = {m2setSpeed , 0, 0, 0.115};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

   enableInterrupt( e1a, risingM1, RISING);
   enableInterrupt( e2b, risingM2, RISING);

   md.setSpeeds(m1setSpeed,m2setSpeed);
    

   Serial.print("Target Ticks: ");
   Serial.println(distanceTicks);

   while(1){
      
      //Serial.print(sqWidthToRPM(squareWidth_M1));
      //Serial.print(" ");
      //Serial.println(sqWidthToRPM(squareWidth_M2));
	  
	  noInterrupts();
      currentTicksM1 = M1ticks;
      currentTicksM2 = M2ticks;
      interrupts();
      
      if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks){
        md.setBrakes(400, 400);
        Serial.print("M1BreakTicks: ");
        Serial.println(currentTicksM1);
		    Serial.print("M2BreakTicks: ");
        Serial.println(currentTicksM2);
        break;
      }
      
      tuneEntryTime = micros();//Can try removing interval for single grid movement
      interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){
        
			if(currentTicksM1 < 0.7*distanceTicks){
			  tuneMotors(rpm, &M1pid, &M2pid);
			}else {
			  tuneMotors(rpm*0.65, &M1pid, &M2pid);
			}
			 
          
        
            tuneExitTime = micros();
        }

    }//end of while
      
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
}

void moveForwardBeta(int rpm, double distance){

   unsigned long tuneEntryTime = 0;
   unsigned long tuneExitTime = 0;
   unsigned long interval = 0;

   double distanceTicks = 1 * distance * ticksPerCM;
   unsigned long currentTicksM1 = 0;
   unsigned long currentTicksM2 = 0;
   
   int m1setSpeed = 100;
   int m2setSpeed = 100;
 
   MotorPID M1pid = {m1setSpeed, 0, 0, 0.1};//0.1=>50
   MotorPID M2pid = {m2setSpeed , 0, 0, 0.115};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

   enableInterrupt( e1a, risingM1, RISING);
   enableInterrupt( e2b, risingM2, RISING);

   md.setSpeeds(m1setSpeed,m2setSpeed);
    

   Serial.print("Target Ticks: ");
   Serial.println(distanceTicks);

   while(1){
      
      //Serial.print(sqWidthToRPM(squareWidth_M1));
      //Serial.print(" ");
      //Serial.println(sqWidthToRPM(squareWidth_M2));
	  
	  noInterrupts();
      currentTicksM1 = M1ticks;
      currentTicksM2 = M2ticks;
      interrupts();
      
      if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks){
        md.setBrakes(400, 400);
        Serial.print("M1BreakTicks: ");
        Serial.println(currentTicksM1);
		Serial.print("M2BreakTicks: ");
        Serial.println(currentTicksM2);
        break;
      }
      
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){
        
			if(currentTicksM1 < 0.7*distanceTicks){
			  tuneM1(rpm, &M1pid);
			  tuneM2(rpm, &M2pid);
			  //tuneMotors(rpm, &M1pid, &M2pid);
			}else if(currentTicksM1 < 0.85*distanceTicks){
			  tuneM1(rpm*0.75, &M1pid);
			  tuneM2(rpm*0.75, &M2pid);
			  //tuneMotors(rpm*0.75, &M1pid, &M2pid);
			}else{
			  tuneM1(rpm*0.4, &M1pid);
			  tuneM2(rpm*0.4, &M2pid);
			  //tuneMotors(rpm*0.4, &M1pid, &M2pid);
			}
          
        
          tuneExitTime = micros();
        }

    }//end of while
      
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
}

  
