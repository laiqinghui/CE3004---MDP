// # Connection:
// #       Pin 1 VCC (URM V3.2) -> VCC (Arduino)
// #       Pin 2 GND (URM V3.2) -> GND (Arduino)
// #       Pin 4 PWM (URM V3.2) -> Pin 3 (Arduino)
// #       Pin 6 COMP/TRIG (URM V3.2) -> Pin 5 (Arduino)
// #
unsigned int Distance = 0;
//uint8_t EnPwmCmd[4] = {0x44, 0x02, 0xbb, 0x01}; // distance measure command through PWM
//uint8_t DMcmd[4] = {0x22, 0x00, 0x00, 0x22}; //distance measure command

void PWM_Mode_Setup()
{
  pinMode(2, OUTPUT);                    // A low pull on pin COMP/TRIG // PWM trigger pin
  digitalWrite(2, HIGH);                 // Set to HIGH

  pinMode(5, INPUT);                      // Sending Enable PWM mode command

  for (int i = 0; i < 4; i++)
  {
    //Serial.write(EnPwmCmd[i]);
  }
}

void Serial_Mode_Setup()
{
  for (int i = 0; i < 4; i++)
  {
    //Serial.write(DMcmd[i]);
  }
}

char getUltraSoundDistance() {

  char USValue = -1;
  Serial_Mode_Setup();
  delay(40); //delay for 75 ms
  unsigned long timer = millis();
  while (millis() - timer < 30)
  {
    if (Serial.available() > 0)
    {
      int header = Serial.read(); //0x22
      int highbyte = Serial.read();
      int lowbyte = Serial.read();
      Serial.read(); //sum

      if (header == 0x22) {
        if (highbyte == 255)
        {
          USValue = -1; //if highbyte =255 , the reading is invalid.
        }
        else
        {
          USValue = highbyte * 255 + lowbyte;
        }

      }
      else {
        while (Serial.available()) Serial.read();
        break;
      }
    }
  }
  return USValue;
}

int getPWMReading(){
  OCR1A = 0;
  delay(20);
  digitalWrite(2, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(2, HIGH);
  //digitalWrite(2, HIGH);               // reading Pin PWM will output pulses

  long value = pulseIn(5, LOW); // PWM Output 0－25000US，Every 50US represent 1cm E1B
  digitalWrite(2, LOW);
  return value/50;
}

//Use motor 1 input A, digital pin 2 as trigger for sensor reading
//Use motor 1 E1B to read sensor output which is digital pin 5
unsigned int getUltraSound2Reading(){
  int count = 0;
  int DistanceMeasured = 0;
  while (DistanceMeasured == 0 || DistanceMeasured >= 10200)
  {
    DistanceMeasured = getPWMReading();
	count++;
	if(count == 20)
		return -3;
		break;
  }
  return DistanceMeasured;         // every 50us low level stands for 1cm
}
