#include <Wire.h>

#define PI_PIN 11

//Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char inBuffer[10] = {0};
char outBuffer[10] = {0};
boolean newData = false;


void printArray(char arr[], int len){
    //Start from one as first char is empty
    Serial.println("In printArray():");
    for(int i = 0; i < len ; i++){
        Serial.print((int)arr[i]);
      }
    Serial.println();
    
  }

void interruptPi(){
    digitalWrite(PI_PIN, HIGH);
    delay(100);
    digitalWrite(PI_PIN, LOW);
    
  }  

void acknowledgeRPI(int len){
  
  outBuffer[0] = 'A'; 
  /*
  for(int i = 0; i < len ; i++){
        outBuffer[i+1] = inBuffer[i];
  }
  */
  interruptPi();
  
}

// callback for received data
void receiveData(int byteCount) {
  Serial.println("Recieving data...");
  
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
    
   acknowledgeRPI(len-1);
	 newData = true;//Set flag for main program to process data
   
  } else {
    
      //Single ack byte return from the Pi after Arduino sent a string over
      //Wire.read() here is required to clear off the the ack byte from the Wire inBuffer in order to accept more new bytes
      Serial.print("Ack byte: ");
      Serial.println(Wire.read());
    
    }
}  // end while



// callback for sending data
void sendData() {
  //char outTest[5] = {'H','E','L','L','O'};
  //Wire.write(outTest);
  Wire.write(outBuffer);
  //printArray(outBuffer, 6);
}

void initI2C(){

	Wire.begin(SLAVE_ADDRESS);
	pinMode(PI_PIN, OUTPUT);
  digitalWrite(PI_PIN, LOW);

  //define callbacks for i2c communication
	Wire.onReceive(receiveData);
	Wire.onRequest(sendData);
	
}


  
char * getinBuffer(){
	
	newData = false;
	return inBuffer;
	
}


void resetInBuffer(){
	
	for(int i = 0; i < 50; i ++ ){
		inBuffer[i] = 0;
	}
	
}


void setOutBuffer(char * data, int len){
  
  for(int i = 0; i < len ; i++){
        outBuffer[i] = data[i];
        Serial.print("outBuffer[");Serial.print(i);Serial.print(": ");Serial.println((int)outBuffer[i]); 
  }
  //outBuffer[5] = '\0';
  
}



boolean dataExist(){
	
	return newData;
	
}
