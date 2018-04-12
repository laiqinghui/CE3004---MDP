#include "Serial.h"
#include "Forward.h"

void processInst() {

  char *instBuff = getinBuffer();//inBuffer can be accessed directly but not a nice practice i think
  boolean fastestPath = false;
  int index = 1;//Start with 1 as first character is sensor flag which is checked after moving
  int moveCount = 0;
  int delayAmount = 150;

  /*
  if(instBuff[0] == 'C')
  {
    fastestPath = true;
  }
  */
  while (instBuff[index] != ';') 
  {
    switch (instBuff[index]) 
    {
      //-----------------Straight----------------------
      case 'W':   moveCount = instBuff[index + 1] - 48;
            if (moveCount == 1){
              moveForwardOneGridBeta();
              
            }  
            else if(moveCount == 2){ 
              moveForwardTwoGridBeta();
              
            }  
            else {
              moveForwardMultipleGridBeta(moveCount);
              
              }  
            moveForwardNumber = moveCount;
            tooCloseToWall();
            //delay(50);
            break;
      
      //---------------Turn Left-----------------------     
      case 'A':   acceptTony = true;
            moveCount = instBuff[index + 1] - 48;
            for (int a = 0; a < moveCount; a++) 
            {
              delay(delayAmount);
              turnPID(-1, 90);
              delay(delayAmount);
            }
            //Reset side wall reading
            resetSideWall();
            break;
      
      //--------------Turn Right----------------------
      case 'D':   acceptTony = true;
            moveCount = instBuff[index + 1] - 48;
            for (int a = 0; a < moveCount; a++) 
            {
              delay(delayAmount);
              turnPID(1, 90);
              delay(delayAmount);
            }
            //Reset side wall reading
            resetSideWall();
            break;
      
      //---------------Turn 180-----------------------
      case 'O': acceptTony = true;
            moveCount = instBuff[index + 1] - 48;
            for (int a = 0; a < moveCount; a++) 
            {
              delay(delayAmount);
              turnPID(1, 90);
              delay(delayAmount);
              turnPID(1, 90);
              delay(delayAmount);
            }
            //Reset side wall reading
            resetSideWall();
            break;
            
      //--------------Corner Calibrate---------------
      case ']':  
            delay(delayAmount);
            fastCalibration(2);
            acceptTony = false;
            break;
            
      //-------------Calibrate Right Wall------------
      case 'R': if(true)
				{
					calibrateBeforeMoveForward();
				}
				break;
            
      //-------------Calibrate Front Wall------------
      case 'F':	delay(delayAmount);
				fastCalibration(0);
				break;
            
      //--------------Initial Calibration--------------
      case '|':	calibration();
				break;
        
      //----------------Sensor Data--------------------
      case 'S':   setOutBuffer('S', getSensorReadingInCM(), 5);
            sendToPi();//Interrupt RPI to notify data is ready
            break;
        
      //--------------Face North-----------------------
      case 'H': delay(5000);
				faceNorthCalibration();
				break;
      default:  ;//do nothing
    } 
    index += 2;
  }
    if(fastestPath)
  {
    interruptPi();
  }
  //Serial.print("No. of move inst: ");
  //Serial.println(index-1);
  if (instBuff[0] == 'S') 
  {
    //Serial.println("Movement done.");
    setOutBuffer('S', getSensorReadingInCM(), 7);
    sendToPi();//Interrupt RPI to notify data is ready
    //Serial.println("Sending done.");
  }

}

//---------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Program Started!!!!");
  md.init();
  //pinMode(PI_PIN, OUTPUT);
  //digitalWrite(PI_PIN, LOW);
  //initI2C();
  PWM_Mode_Setup();
  //calibrateReading();

}

void loop(){
  //printSensorValue();
  if (dataExist()) {
    processInst();
  }
  if(Serial.available())
  {
    receiveData();
  }
  
  
}

void calibrateReading()
{
  int listOfReadings1[20];
  int listOfReadings2[20];
  for(int a = 0; a<20; a++)
  {
  listOfReadings1[a] = getIRSensorReading()[3];
  listOfReadings2[a] = getIRSensorReading()[5];
  delay(10);
  }
  Serial.println("Set");
  Serial.println("Reading 1");
  Serial.println(sortAndAverage(listOfReadings1, 20, 3));
  Serial.println("Reading 2");
  Serial.println(sortAndAverage(listOfReadings2, 20, 3));
}

void printSensorValue()
{
	//sensorValues[0] FL
    //sensorValues[1] FR
    //sensorValues[2] L
    //sensorValues[3] R
    //sensorValues[4] CL
    //sensorValues[5] CR
	Serial.println(getIRSensorReading()[4]);
}

