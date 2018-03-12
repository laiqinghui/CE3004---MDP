//------------Other constants and declarations----------
signed long wheelDiameter = 18.849556; //Diameter is 6cm
signed long ticksPerCM = 562.215 / wheelDiameter;

DualVNH5019MotorShield md(2, 4, 6, A0, 7, 8, 12, A1);

//------------Wheel Encoders constants------------
#define e1a 3
#define e1b 5
#define e2a 11
#define e2b 13
