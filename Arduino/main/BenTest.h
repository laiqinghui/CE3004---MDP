#include <Wire.h>

#define PI_PIN 11

//Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char inBuffer[32] = {0};
char outBuffer[32] = {0};
boolean newData = false;
int instCount = 0;


void printArray(char arr[], int len) {

  for (int i = 0; i < len ; i++) {
    Serial.print((int)arr[i]);
    Serial.print(" ");
    Serial.print(arr[i]);
    Serial.print(", ");
  }
  Serial.println();

}

void interruptPi() {
  digitalWrite(PI_PIN, HIGH);
  delay(100);
  digitalWrite(PI_PIN, LOW);
  sendData();

}

void setOutBuffer(char opcode, char * data, int len) {

  outBuffer[0] = opcode;
  //Serial.print("Outbuffer set to op: ");
  //Serial.println(opcode);
  for (int i = 0; i < len ; i++) {
    outBuffer[i + 1] = data[i];
  }

}


// callback for received data
void receiveData() {


	int i = 0;
	int len = Serial.available()

	if (len > 1) 
	{
		Serial.read();//Clear 1st garbage byte	
		while (i < len) 
		{
			// read the incoming byte:
			inBuffer[i] = Serial.read();
			i++;
		}
	}
    newData = true;//Set flag for main program to process data
	else 
	{
		Serial.read();//Clear 1st garbage byte	
	}
}



// callback for sending data
void sendData() {

  Serial.write(outBuffer);

}

void initI2C() {

  Wire.begin(SLAVE_ADDRESS);
  pinMode(PI_PIN, OUTPUT);
  digitalWrite(PI_PIN, LOW);

  //define callbacks for i2c communication
  //Wire.onReceive(receiveData);
  //Wire.onRequest(sendData);

}



char* getinBuffer() {

  newData = false;
  return inBuffer;

}


void resetInBuffer() {

  for (int i = 0; i < 10; i ++ ) {
    inBuffer[i] = 0;
  }

}



boolean dataExist() {

  return newData;

}
