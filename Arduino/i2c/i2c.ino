//Import the library required
#include <Wire.h>
#define PI_PIN A0

/*
  I2C Pinouts
  SDA -> A4
  SCL -> A5
*/

//Slave Address for the Communication
#define SLAVE_ADDRESS 0x04

char buffer[50] = {0};

void setup() {
  
  // initialize i2c as slave
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  pinMode(PI_PIN, OUTPUT);
  digitalWrite(PI_PIN, LOW);
 
  
  // define callbacks for i2c communication
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  
}

void loop() {
  //Wire.write("Data From Arduino!");
  //delay(500);
} // end loop

void printArray(char arr[], int len){
    //Start from one as first char is empty
    for(int i = 1; i < len ; i++){
        Serial.print(arr[i]);
      }
    Serial.println();
    
  }
  
// callback for received data
void receiveData(int byteCount) {
  int i = 0;
  int len = Wire.available();
  Serial.print("Incoming: ");
  Serial.print(len);
  Serial.println(" bytes");

  
  if(len > 1){
    while (i<len) {
      
      buffer[i] = Wire.read();
      
      //Serial.print("buffer[i]: ");
      //Serial.println(buffer[i]);
      i++;
  
    }
    
    Serial.print("buffer: ");
    printArray(buffer, len);
    interruptPi();
    //sendData();
  } else {
    
      //Single ack byte return from the Pi after Arduino sent a string over
      //Wire.read() here is required to clear off the the ack byte from the Wire Buffer in order to accept more new bytes
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
  Serial.println("Request incoming from PI!");
  Wire.write("Data From Arduino!");
}



