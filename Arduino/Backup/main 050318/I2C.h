#include <Wire.h>

#define PI_PIN 11

//Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char inBuffer[32] = {0};
char outBuffer[32] = {0};
boolean newData = false;
int instCount = 0;


void printArray(char arr[], int len){
    
    for(int i = 0; i < len ; i++){
        Serial.print((int)arr[i]);
        Serial.print(" ");
        Serial.print(arr[i]);
        Serial.print(", ");
      }
    Serial.println();
    
  }

void interruptPi(){
    digitalWrite(PI_PIN, HIGH);
    delay(100);
    digitalWrite(PI_PIN, LOW);
    
  }  

void setOutBuffer(char opcode, char * data, int len){

  outBuffer[0] = opcode;
  Serial.print("Outbuffer set to op: ");
  Serial.println(opcode);
  for(int i = 0; i < len ; i++){
        outBuffer[i+1] = data[i]; 
  }
  
}

void acknowledgeRPI(int len){
  
  setOutBuffer('A', inBuffer, 0);//Not ecohing inst back anymore
  interruptPi();
  
}

// callback for received data
void receiveData(int byteCount) {

  
  int i = 0; 
  int len = Wire.available();
 
  
  Serial.print("Incoming: ");
  Serial.print(len);
  Serial.println(" bytes");

  if(len > 1){
    
	  Wire.read();//Clear 1st garbage byte
    while (i<len) {
      
      inBuffer[i] = Wire.read();
      i++;
  
    }
   Serial.print("\n\nInstruction ");
   Serial.print(++instCount);
   Serial.println(":"); 
   printArray(inBuffer, len -1);
   acknowledgeRPI(len-1);
 
      
	 newData = true;//Set flag for main program to process data
   
  } else {
    
      //Single ack byte return from the Pi after Arduino sent a string over
      //Wire.read() here is required to clear off the the ack byte from the Wire inBuffer in order to accept more new bytes
      Wire.read();
    }
}  // end while



// callback for sending data
void sendData() {

  Wire.write(outBuffer);
  
}

void initI2C(){

	Wire.begin(SLAVE_ADDRESS);
	pinMode(PI_PIN, OUTPUT);
  digitalWrite(PI_PIN, LOW);

  //define callbacks for i2c communication
	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);
	
}


  
char* getinBuffer(){
	
	newData = false;
	return inBuffer;
	
}


void resetInBuffer(){
	
	for(int i = 0; i < 10; i ++ ){
		inBuffer[i] = 0;
	}
	
}



boolean dataExist(){
	
	return newData;
	
}
