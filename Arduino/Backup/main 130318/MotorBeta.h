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
   
   Serial.print("Target Ticks: ");
   Serial.println(distanceTicks);
   
   unsigned long currentTicksM1 = 0;
   unsigned long currentTicksM2 = 0;
   unsigned long previousTicksM1 = 0;
   unsigned long previousTicksM2 = 0;
   int ticksToCoverUp = 0;
   
   int m1setSpeed = 100;
   int m2setSpeed = 100;
 
   MotorPID M1pid = {m1setSpeed, 0, 0, 0.110};//LOUNGE: 0.110=>50/ LAB: 0.115=>50 || LAB: 0.110=>90/ LAB: 0.115=>50
   MotorPID M2pid = {m2setSpeed , 0, 0, 0.138};//LOUNGE: 0.150->50/LAB: 0.150->50 || LAB: 0.135=>90/ LAB: 

   calibrateBeforeMoveForward();
    
   enableInterrupt( e1a, risingM1, RISING);
   enableInterrupt( e2b, risingM2, RISING);

   
   md.setSpeeds(m1setSpeed,m2setSpeed);

   while(1){
      
      //Serial.print(sqWidthToRPM(squareWidth_M1));
      //Serial.print(" ");
      //Serial.println(sqWidthToRPM(squareWidth_M2));
	  
	    noInterrupts();
      currentTicksM1 = M1ticks;
      currentTicksM2 = M2ticks;
      interrupts();
      previousTicksM1 = currentTicksM1;
      previousTicksM2 = currentTicksM2;

      
      if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks){
        md.setBrakes(400, 400);
        /*
        Serial.print("M1BreakTicks: ");
        Serial.println(currentTicksM1);
		    Serial.print("M2BreakTicks: ");
        Serial.println(currentTicksM2);
        */
        if( (distanceTicks - currentTicksM1) > 3 ){ //Only cover the ticks if is more then 3 difference
          
          ticksToCoverUp = distanceTicks - currentTicksM1;
          setTicks(0,0);
          
          while(1){
              //Serial.print("M1Ticks: ");
              //Serial.println(currentTicksM1);
              md.setSpeeds(5, 0);
              noInterrupts();
              currentTicksM1 = M1ticks;
              interrupts();
              if(currentTicksM1 >= ticksToCoverUp){
                //Serial.print("M1BreakTicks: ");
                //Serial.println(currentTicksM1);
                break;
                }
            }
            md.setBrakes(400, 400);
            
        }else if( (distanceTicks - currentTicksM2) > 3 ){
          ticksToCoverUp = distanceTicks - currentTicksM2;
          setTicks(0,0);
          while(1){
              md.setSpeeds(0, 5);
              noInterrupts();
              currentTicksM2 = M2ticks;
              interrupts();
              if(currentTicksM2 >= ticksToCoverUp){
                //Serial.print("M2BreakTicks: ");
                //Serial.println(currentTicksM2);
                break;
                }
            }
            md.setBrakes(400, 400);
            }
        break;
      }
      
      tuneEntryTime = micros();//Can try removing interval for single grid movement
      interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){ //change to 3000 to try
        
			//if(currentTicksM1 < 0.7*distanceTicks){
			  tuneMotors(rpm, &M1pid, &M2pid);
			//}else {
			  //tuneMotors(rpm*0.65, &M1pid, &M2pid);
			//}
			 
          
        
            tuneExitTime = micros();
        }

    }//end of while
      
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
      
}

void moveForwardBeta(int rpm, double distance){

   
   double rightFrontSensor = 0;
   unsigned long tuneEntryTime = 0;
   unsigned long tuneExitTime = 0;
   unsigned long interval = 0;

   double distanceTicks = 1 * distance * ticksPerCM;
   unsigned long currentTicksM1 = 0;
   unsigned long currentTicksM2 = 0;
   
   int m1setSpeed = 100;
   int m2setSpeed = 100;
 
   MotorPID M1pid = {m1setSpeed, 0, 0, 0.110};//0.1=>50
   MotorPID M2pid = {m2setSpeed , 0, 0, 0.122};//0.163=>50 0.134=>80 0.128=>90 /// Bat2: 0.119 => 90rpms

   enableInterrupt( e1a, risingM1, RISING);
   enableInterrupt( e2b, risingM2, RISING);

   md.setSpeeds(m1setSpeed,m2setSpeed);
    

   Serial.print("Target Ticks: ");
   Serial.println(distanceTicks);

   while(1){
      
      /*
      rightFrontSensor = getSideCalibrationReading(true)[0];
      if(rightFrontSensor < 10 || (rightFrontSensor > 13)){
            fastCalibration(1);
        }
      */ 
	    noInterrupts();
      currentTicksM1 = M1ticks;
      currentTicksM2 = M2ticks;
      interrupts();
      
      if(currentTicksM1>=distanceTicks || currentTicksM2>=distanceTicks){
        md.setBrakes(400, 400);
        /*
        Serial.print("M1BreakTicks: ");
        Serial.println(currentTicksM1);
		    Serial.print("M2BreakTicks: ");
        Serial.println(currentTicksM2);
        */
        break;
      }
      
      tuneEntryTime = micros();
        interval = tuneEntryTime - tuneExitTime;
        if(interval >= 5000){
        
			if(currentTicksM1 < 0.7*distanceTicks){
			  //tuneM1(rpm, &M1pid);
			  //tuneM2(rpm, &M2pid);
			  tuneMotors(rpm, &M1pid, &M2pid);
			}else if(currentTicksM1 < 0.85*distanceTicks){
			  //tuneM1(rpm*0.75, &M1pid);
			  //tuneM2(rpm*0.75, &M2pid);
			  tuneMotors(rpm*0.75, &M1pid, &M2pid);
			}else{
			  //tuneM1(rpm*0.4, &M1pid);
			  //tuneM2(rpm*0.4, &M2pid);
			  tuneMotors(rpm*0.4, &M1pid, &M2pid);
			}
          tuneExitTime = micros();
        }

    }//end of while
      
      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
}

void turn90PIDBeta(int dir){

    double cir = 3.141 * 17.6; //circumfrence of circle drawn when turning in cm, current diameter used is 17.6
    double cmToCounts = singlerevticks/(6*3.141); //cm to counts for wheel
    int amount = cir * (90/360) * cmToCounts;//int to ignored decimal value
    unsigned long currentTicksM1 = 0;
    unsigned long currentTicksM2 = 0;
    int tuneSpeedM1 = 0;
    int tuneSpeedM2 = 0;
    int m1Speed = dir * 214;
    int m2Speed = dir * 255;

    unsigned long tuneEntryTime = 0;
    unsigned long tuneExitTime = 0;
    unsigned long interval = 0;
    
    

   if(dir == 1){//Turn right(left motor(M2) forward). Tune M2 to match M1.

      MotorPID M2 = {m2Speed , 0, 0, 0.1}; 
      enableInterrupt( e1a, risingM1Ticks, RISING);
      enableInterrupt( e2b, risingM2Ticks, RISING);
      md.setSpeeds(m1Speed, m2Speed);
     
      while(currentTicksM1 < amount)
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;
          
          if(interval >= 5000){ 
            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M2.currentErr =  currentTicksM1 - currentTicksM2; //Positive means M1 is faster
            tuneSpeedM2 = M2.prevTuneSpeed + M2.gain*M2.currentErr + (M2.gain/0.05)*(M2.currentErr - M2.prevErr1);
            md.setSpeeds(m1Speed, tuneSpeedM2);
            
            M2.prevTuneSpeed = tuneSpeedM2;
            M2.prevErr1 = M2.currentErr;
            Serial.print("M2.currentErr");
            Serial.println(M2.currentErr);
          }//end of if
          
        }// end of while
    }//end of if
    else //turn left(right motor(M1) forward). Tune M1 to match M2. 
    {
      
      MotorPID M1 = {m1Speed , 0, 0, 0.1}; 
      enableInterrupt( e1a, risingM1Ticks, RISING);
      enableInterrupt( e2b, risingM2Ticks, RISING);
      md.setSpeeds(m1Speed, m2Speed);
     
      while(currentTicksM2 < amount)
        {
          tuneEntryTime = micros();
          interval = tuneEntryTime - tuneExitTime;
          
          if(interval >= 5000){ 
            noInterrupts();
            currentTicksM1 = M1ticks;
            currentTicksM2 = M2ticks;
            interrupts();
      
            M1.currentErr = currentTicksM2 - currentTicksM1; //Positive means M1 is faster
            tuneSpeedM1 = M1.prevTuneSpeed + M1.gain*M1.currentErr + (M1.gain/0.05)*(M1.currentErr - M1.prevErr1);
            md.setSpeeds(tuneSpeedM1, m2Speed);
            
            M1.prevTuneSpeed = tuneSpeedM1;
            M1.prevErr1 = M1.currentErr;
            Serial.print("M1.currentErr");
            Serial.println(M1.currentErr);
          }
          
        }// end of while
  }

      disableInterrupt(e1a);
      disableInterrupt(e2b);
      setTicks(0,0);
      setSqWidth(0,0);
      
  }// end of function


  
