/*
 sketch.ino

 Stephen Hailes, Oct 2020

 This is a template sketch for the testboard simduino
  
 */

#include <stdio.h>
#include <string.h>

void heat(void);
double get_difference(double value,double setvalue);
double get_heat_pid(void);
double get_temp(void);
double get_settemp(void);
double get_output(double setspeed);

// These define which pins are connected to what device on the virtual bioreactor
//
const byte lightgatePin  = 2;
const byte stirrerPin    = 5;
const byte heaterPin     = 6;
const byte thermistorPin = A0;
const byte pHPin         = A1;

// The PCA9685 is connected to the default I2C connections. There is no need
// to set these explicitly.

double settemp = 302;
double heat_error = 0;

const double total_vol = 5.0;
const double internal_R = 10000.0;
const double B = 4220.0;
const double R_25 = 10000.0;
const double t = 25.0+273.15;


void setup() {
  Serial.begin(9600);

  pinMode(lightgatePin,  INPUT);
  pinMode(stirrerPin,    OUTPUT);
  pinMode(heaterPin,     OUTPUT);
  pinMode(thermistorPin, INPUT);
  pinMode(pHPin,         INPUT);
 
}

 
void loop() {
  heat();
}

//------------------------------------------------------------------------------

void heat(){
  double temp = get_temp();
  heat_error = get_difference(temp,settemp);
  if (heat_error > 5.0){
    analogWrite(heaterPin, 200);
  } else if (heat_error > 3.0){
    analogWrite(heaterPin, 150);
  } else if (heat_error > 1.0){
    analogWrite(heaterPin, 125);
  } else if (heat_error > 0.5){
    analogWrite(heaterPin, 60);
  } else{
    analogWrite(heaterPin, 0);
  }
}

//------------------------------------------------------------------------------

double get_difference(double value,double setvalue){
  return setvalue - value;
}

//------------------------------------------------------------------------------

double get_temp(){
  double analog_vol = analogRead(thermistorPin);
  double vol = (analog_vol/1023)*total_vol;
  double R = vol*internal_R/(total_vol-vol);
  double temp = (B*t)/(B+(log(R/R_25)*t));
  return temp;
}
 

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------


