#include <Wire.h>

#define PI_PIN 11

//Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char inBuffer[50] = {0};
char outBuffer[50] = {0};
boolean newData = false;


void printArray(char arr[], int len){
    //Start from one as first char is empty
    for(int i = 1; i < len ; i++){
        Serial.print(arr[i]);
      }
    Serial.println();
    
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
    
   Serial.print("inBuffer: ");
   printArray(inBuffer, len);
	 newData = true;
   
  } else {
    
      //Single ack byte return from the Pi after Arduino sent a string over
      //Wire.read() here is required to clear off the the ack byte from the Wire inBuffer in order to accept more new bytes
      Serial.print("Ack byte: ");
      Serial.println(Wire.read());
    
    }
}  // end while

void interruptPi(){
    digitalWrite(PI_PIN, HIGH);
    delay(500);
    digitalWrite(PI_PIN, LOW);
    
  }

// callback for sending data
void sendData() {
  Wire.write("HELLO THIS IS ARDUINO");
}

void initI2C(){

	Wire.begin(SLAVE_ADDRESS);
	pinMode(PI_PIN, OUTPUT);//Need to set this as output on demand 
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

boolean dataExist(){
	
	return newData;
	
}
