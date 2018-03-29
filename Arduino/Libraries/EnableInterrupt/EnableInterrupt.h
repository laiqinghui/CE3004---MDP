typedef void (*interruptFunctionType)(void);

volatile interruptFunctionType interruptFunctions[2]; 
volatile int pinNum[2];

volatile unsigned long M1ticks = 0;
volatile unsigned long M2ticks = 0;

volatile unsigned long breakTicks = 0;

volatile int numberOfBrakes = 0;

void setBrakes(){	
  //noInterrupts();
  PORTD = PORTD & B01101011;
  PORTB = PORTB & B11111110;
  
	
  OCR1A = 400;
  OCR1B = 400;
  
  //interrupts();
  
}

void enableInterrupt(uint8_t pinNumber, void (*func)(void), uint8_t mode)
{
	PCICR |= 0b00000101;    // turn on ISR PCINT2 and PCINT0
	mode = mode;
	pinMode(pinNumber,INPUT);   // set Pin as Input (default)
	digitalWrite(pinNumber,HIGH);
	
	PCMSK2 |= 0b00001000;    // turn on pins 3
	PCMSK0 |= 0b00100000;    // turn on pin 13
	
	//ISR (PCINT2_vect)
	if(pinNumber <8)
	{
		interruptFunctions[0] = func;
		pinNum[0] = pinNumber;
	}
	//ISR (PCINT0_vect)
	else
	{
		interruptFunctions[1] = func;
		pinNum[1] = pinNumber;
	}
}

void disableInterrupt(uint8_t pinNumber){
	//PCICR &= 0b00000010;    // turn off ISR PCINT2 and PCINT0
	//ISR (PCINT2_vect)
	if(pinNumber <8)
	{
		PCMSK2 &= 0b11110111;    // turn off pins 3
	
		interruptFunctions[0] = NULL;
		pinNum[0] = -1;
	}
	//ISR (PCINT0_vect)
	else
	{
		PCMSK0 &= 0b11011111;    // turn off pin 13
		interruptFunctions[1] = NULL;
		pinNum[1] = -1;
	}
}

volatile boolean movementDone = false;

ISR(PCINT0_vect)
{
	sei();
	int value = PINB;
	int currentBit = (value >> (pinNum[1] - 8))% 2;
	if(currentBit == 1)
	{
		(*interruptFunctions[1])();
		
	}
	M2ticks++;
	if(M2ticks >= breakTicks)
	{
		setBrakes();
		movementDone = true;
    }
}

ISR(PCINT2_vect)
{
	sei();
	int value = PIND;
	int currentBit = (value >> pinNum[0])% 2;
	if(currentBit == 1)
	{
		(*interruptFunctions[0])();
		
	}
	M1ticks++;
	if(M1ticks >= breakTicks)
	{
		setBrakes();
		movementDone = true;
	}
}

void setTicks(int M1, int M2) {
  M1ticks = M1;
  M2ticks = M2;
}



