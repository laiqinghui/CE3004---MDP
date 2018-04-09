#define PI_PIN 11

//Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char inBuffer[128] = {0};
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

void sendToPi() {
  Serial.write(outBuffer);
}

void interruptPi() {
  digitalWrite(PI_PIN, HIGH);
  delay(50);
  digitalWrite(PI_PIN, LOW);
}

void setOutBuffer(char opcode, char * data, int len) {
	
  outBuffer[0] = '$';
  outBuffer[1] = opcode;
  for (int i = 0; i < len ; i++) {
    outBuffer[i + 2] = data[i];
  }

}


// callback for received data
void receiveData() {
   
  char received = 'a';
  int i = 0;
  while(received != ';')
  {
    if(Serial.available()){
      received = Serial.read();
      inBuffer[i++] = received;
    }
  }
  
  if(i != 0)
  {
    newData = true;//Set flag for main program to process data
  }
}

// callback for sending data
void sendData() {
  Serial.write(outBuffer);
}

char* getinBuffer() {
  newData = false;
  return inBuffer;
}

void resetInBuffer() {

  for (int i = 0; i < 128; i ++ ) {
    inBuffer[i] = 0;
  }
}

boolean dataExist() {

  return newData;

}

