#include "DualVNH5019MotorShield.h"

// Constructors ////////////////////////////////////////////////////////////////

DualVNH5019MotorShield::DualVNH5019MotorShield()
{
  //Pin map
  _INA1 = 2;
  _INB1 = 4;
  _EN1DIAG1 = 6;
  _CS1 = A0; 
  _INA2 = 7;
  _INB2 = 8;
  _EN2DIAG2 = 12;
  _CS2 = A1;
}

DualVNH5019MotorShield::DualVNH5019MotorShield(unsigned char INA1, unsigned char INB1, unsigned char EN1DIAG1, unsigned char CS1, 
                                               unsigned char INA2, unsigned char INB2, unsigned char EN2DIAG2, unsigned char CS2)
{
  //Pin map
  //PWM1 and PWM2 cannot be remapped because the library assumes PWM is on timer1
  _INA1 = INA1;
  _INB1 = INB1;
  _EN1DIAG1 = EN1DIAG1;
  _CS1 = CS1;
  _INA2 = INA2;
  _INB2 = INB2;
  _EN2DIAG2 = EN2DIAG2;
  _CS2 = CS2;
}

// Public Methods //////////////////////////////////////////////////////////////
void DualVNH5019MotorShield::init()
{
// Define pinMode for the pins and set the frequency for timer1.

  pinMode(_INA1,OUTPUT);
  pinMode(_INB1,OUTPUT);
  pinMode(_PWM1,OUTPUT);
  pinMode(_EN1DIAG1,INPUT);
  //pinMode(_CS1,INPUT);
  pinMode(_INA2,OUTPUT);
  pinMode(_INB2,OUTPUT);
  pinMode(_PWM2,OUTPUT);
  pinMode(_EN2DIAG2,INPUT);
  //pinMode(_CS2,INPUT);
  #if defined(__AVR_ATmega168__)|| defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
  // Timer 1 configuration
  // prescaler: clockI/O / 1
  // outputs enabled
  // phase-correct PWM
  // top of 400
  //
  // PWM frequency calculation
  // 16MHz / 1 (prescaler) / 2 (phase-correct) / 400 (top) = 20kHz
  TCCR1A = 0b10100000;
  TCCR1B = 0b00010001;
  ICR1 = 400;
  #endif
}
// Set speed for motor 1, speed is a number betwenn -400 and 400
void DualVNH5019MotorShield::setM1Speed(int speed)
{
  unsigned char reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  if (speed > 400)  // Max PWM dutycycle
    speed = 400;

  OCR1A = speed;

  if (speed == 0)
  {
	PORTD = PORTD & B11101011;  
	  
  }
  else if (reverse)
  {
	PORTD = PORTD & B11101111; //Set to low
	PORTD = PORTD | B00000100; //Set to high
  }
  else
  {
	PORTD = PORTD & B11101111; //Set to low
	PORTD = PORTD | B00000100; //Set to high
  }
}

// Set speed for motor 2, speed is a number betwenn -400 and 400
void DualVNH5019MotorShield::setM2Speed(int speed)
{
  unsigned char reverse = 0;
  
  if (speed < 0)
  {
    speed = -speed;  // make speed a positive quantity
    reverse = 1;  // preserve the direction
  }
  if (speed > 400)  // Max 
    speed = 400;

  OCR1B = speed;
  if (speed == 0)
  {
	PORTD = PORTD & B01111111;
	PORTB = PORTB & B11111110;
  }
  else if (reverse)
  {
	PORTD = PORTD & B01111111; //Set to low
	PORTB = PORTB | B00000001; //Set to high  
  }
  else
  {
	PORTD = PORTD | B10000000; //Set to high
	PORTB = PORTB & B11111110; //Set to low
  }
}

// Set speed for motor 1 and 2
void DualVNH5019MotorShield::setSpeeds(int m1Speed, int m2Speed)
{
  //setM1Speed(m1Speed);
  //setM2Speed(m2Speed);
  
  unsigned char reverse = 0;
  if (m1Speed < 0 && m2Speed < 0)
  {
    m1Speed = -m1Speed;  // Make speed a positive quantity
	m2Speed = -m2Speed;  // Make speed a positive quantity
    reverse = 1;  // Preserve the direction
  }
  else if(m1Speed < 0 && !(m2Speed < 0))
  {
	m1Speed = -m1Speed;  // Make speed a positive quantity
    reverse = 2;  // Preserve the direction
  }
  else if(m2Speed < 0 && !(m1Speed < 0))
  {
	m2Speed = -m2Speed;  // Make speed a positive quantity
    reverse = 3;  // Preserve the direction
  }
  
  if (m1Speed > 400)  // Max PWM dutycycle
    m1Speed = 400;
  if (m2Speed > 400)  // Max PWM dutycycle
    m2Speed = 400;

  PORTD = PORTD & B01101011;
	
  PORTB = PORTB & B11111110;	
	
  noInterrupts();
  OCR1B = 400;
  OCR1A = 400;
      
	PORTD = PORTD & B01101011;
	
	PORTB = PORTB & B11111110;
	
	delay(20);
  //both speed are positive
  if (reverse == 0)
  {
	  
	PORTD = PORTD & B11101111; //Set to low	
	PORTD = PORTD | B10000100; //Set to high
	PORTB = PORTB & B11111110; //Set to low
	
	  
	//digitalWrite(_INA1,HIGH);
    //digitalWrite(_INB1,LOW);
	//digitalWrite(_INA2,HIGH);
    //digitalWrite(_INB2,LOW);  
  }
  //both speed are negative
  else if(reverse == 1)
  {
	PORTD = PORTD & B01111011; //Set to low
	PORTD = PORTD | B00010000; //Set to high
	
	PORTB = PORTB | B00000001; //Set to high  
	  
	//digitalWrite(_INA1,LOW);
    //digitalWrite(_INB1,HIGH);
	//digitalWrite(_INA2,LOW);
    //digitalWrite(_INB2,HIGH);
  }
  //m1Speed is negative
  else if(reverse == 2)
  {
	PORTD = PORTD & B11111011; //Set to low
	PORTD = PORTD | B10010000; //Set to high
	
	PORTB = PORTB & B11111110; //Set to low    
	  
    //digitalWrite(_INA1,LOW);
    //digitalWrite(_INB1,HIGH);
    //digitalWrite(_INA2,HIGH);
    //digitalWrite(_INB2,LOW);
  }
  //m2Speed is negative
  else
  {
	PORTD = PORTD & B01101111; //Set to low
	PORTD = PORTD | B00000100; //Set to high
	
	PORTB = PORTB | B00000001; //Set to high      
	  
    //digitalWrite(_INA1,HIGH);
    //digitalWrite(_INB1,LOW);
	//digitalWrite(_INA2,LOW);
    //digitalWrite(_INB2,HIGH);
  }
  OCR1B = m2Speed;
  OCR1A = m1Speed;
  

  interrupts();
}

// Brake motor 1, brake is a number between 0 and 400
void DualVNH5019MotorShield::setM1Brake(){
	 PORTD = PORTD & B11101011;
	 OCR1A = 400;
}

// Brake motor 2, brake is a number between 0 and 400
void DualVNH5019MotorShield::setM2Brake(){
  PORTD = PORTD & B01111111;
  PORTB = PORTB & B11111110;
  OCR1B = 400;
}

// Brake motor 1 and 2, brake is a number between 0 and 400
void DualVNH5019MotorShield::setBrakes(){	
  noInterrupts();
  PORTB = PORTB & B11111110;
  PORTD = PORTD & B01101011;
	
  OCR1A = 400;
  OCR1B = 400;
  interrupts();
}

// Return error status for motor 1 
unsigned char DualVNH5019MotorShield::getM1Fault()
{
  return !digitalRead(_EN1DIAG1);
}

// Return error status for motor 2 
unsigned char DualVNH5019MotorShield::getM2Fault()
{
  return !digitalRead(_EN2DIAG2);
}