//Contains the Mechaduino parameter declarations

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__
//----Identifier-----
extern const String identifier;

//----Sample Frequenz-----
extern volatile float Fs;

//----Filter Coeffizienten-----
extern const float coeff_b0;
extern const float coeff_b1;
extern const float coeff_b2;

extern const float coeff_a1;
extern const float coeff_a2;

//----PID Values-----
extern volatile float pKp;
extern volatile float pKi;
extern volatile float pKd;

//////////////////////////////////////
//////////////////PINS////////////////
//////////////////////////////////////

extern const int IN_4;
extern const int IN_3;
extern const int VREF_2;
extern const int VREF_1;
extern const int IN_2;
extern const int IN_1;
extern const int ledPin;
extern const int chipSelectPin;

extern const int dir_pin;
extern const int step_pin;
extern const int ena_pin;

extern const int steps_per_revolution;
extern const int counts_per_revolution;

extern const float angle_per_step;
extern const float stepangle;

extern int dir;		//initialize stepping mode variables

extern long angle; //holds processed angle value

extern float anglefloat;

extern int a;  // raw encoder value in closed loop and print_angle routine (should fix the latter to include LUT)

extern int stepNumber; // step index for cal routine

extern const PROGMEM float sine_lookup[];
extern const PROGMEM float lookup[];

extern const int overflow;
#endif
