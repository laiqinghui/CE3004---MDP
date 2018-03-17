#include "I2C.h"
#include "Forward.h"

void processInst() {

  char *instBuff = getinBuffer();//inBuffer can be accessed directly but not a nice practice i think
  boolean fastestPath = false;
  int index = 1;//Start with 1 as first character is sensor flag which is checked after moving
  char num[1] = {0};// For checklist
  int moveCount = 0;
  int delayAmount = 250;
  
  if(instBuff[0] == 'C')
  {
	  fastestPath = true;
  }

  while (instBuff[index] != ';') {
    switch (instBuff[index]) {

      case 'W': moveCount = instBuff[index + 1] - 48;
				//delay(delayAmount);
				if (moveCount == 1)
				  moveForwardOneGrid(110);
				else //moveForward(90, 9.5*moveCount, true);
				  moveForwardBeta(110, 9.7 * moveCount);
				break;
      case 'A': moveCount = instBuff[index + 1] - 48;
				for (int a = 0; a < moveCount; a++) {
				  delay(delayAmount);
				  turnPID(-1, 90);
          //turn90PIDBeta(-1);
				  delay(delayAmount);
				}
				//Reset side wall reading
				resetSideWall();
				break;
      case 'D': moveCount = instBuff[index + 1] - 48;
        for (int a = 0; a < moveCount; a++) {
          delay(delayAmount);
		      turnPID(1, 90);
          //turn90PIDBeta(1);
          delay(delayAmount);
        }
        //Reset side wall reading
        resetSideWall();
        break;
      case 'O': moveCount = instBuff[index + 1] - 48;
        for (int a = 0; a < moveCount; a++) {
          turnPID(1, 90);
          //turn90PIDBeta(1);
          delay(delayAmount);
          turnPID(1, 90);
          //turn90PIDBeta(1);
          delay(delayAmount);
        }
        //Reset side wall reading
        resetSideWall();
        break;
      case ']': fastCalibration(2);
        break;
      case 'R': fastCalibration(1);
        break;
      case 'F': fastCalibration(0);
        break;
      case '|': calibration();
        break;
      case 'T': turnPID(-1, atoi(instBuff + 2));
        break;
      case 'S': setOutBuffer('S', getSensorReadingInCM(), 5);
        interruptPi();//Interrupt RPI to notify data is ready
        break;
		
	  case 'H': faceNorthCalibration();
				break;
      case 'I': num[0] = atoi(instBuff + 2) + 1;
        //Serial.print("num = ");
        //Serial.println(num[0]);
        //setOutBuffer('S', num, 1);
        //interruptPi();
        break;
      default:  ;//do nothing

    }	

    index += 2;
    //if(instBuff[index] != ';')
    //delay(200);

  }
  	if(fastestPath)
	{
		interruptPi();
	}
  
  //Serial.print("No. of move inst: ");
  //Serial.println(index-1);
  if (instBuff[0] == 'S') {

    setOutBuffer('S', getSensorReadingInCM(), 5);
    //printArray(outBuffer, 6);
    interruptPi();//Interrupt RPI to notify data is ready
  }


}

//---------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial.println("Program Started!!!!");
  md.init();
  initI2C();
  

  

    EEPROM.write(4, ((int)(0.8975 * 10000)) >> 8);
	EEPROM.write(5, ((int)(0.8975 * 10000)) & 0xFF);
  
	EEPROM.write(6, ((int)(0.8975 * 10000)) >> 8);
	EEPROM.write(7, ((int)(0.8975 * 10000)) & 0xFF);


  //Load in the offset for turn
  unsigned int turnRight90Offset1 = EEPROM.read(4);
  unsigned int turnRight90Offset2 = EEPROM.read(5);
  offsetRight = ((signed int)((turnRight90Offset1 << 8) | turnRight90Offset2)) / 10000.0;
  Serial.println(offsetRight,6);
  
  unsigned int turnLeft90Offset1 = EEPROM.read(6);
  unsigned int turnLeft90Offset2 = EEPROM.read(7);
  offsetLeft = ((signed int)((turnLeft90Offset1 << 8) | turnLeft90Offset2)) / 10000.0;
  Serial.println(offsetLeft,6);
  
}

void loop()
{
	//Serial.println(getIRSensorReading()[0]);
  if (dataExist()) {
    //delay(100);//Delay for ack packet to be sent out. To allow RPI to request and recieve data before we start moving which will affect interrupt operations
    processInst();
  }
}


