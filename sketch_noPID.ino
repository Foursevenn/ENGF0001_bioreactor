/*
 sketch.ino

 Stephen Hailes, Oct 2020

 This is a template sketch for the testboard simduino
  
 */

#include <stdio.h>
#include <string.h>

void heat(void);
void stir(void);
double get_difference(double value,double setvalue);
double get_temp(void);
double get_speed(void);
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

double settemp = 303.5;
double heat_error = 0;

double setspeed = 1000;
double stir_error = 0;

double setpH = 5.0;

double last_time = 0;
double time = 0;
double frequency = 0.0;
const double total_vol = 5.0;
const double internal_R = 10000.0;
const double B = 4220.0;
const double R_25 = 10000.0;
const double t = 25.0+273.15;
double pHnow = 0; 

void setup() {
  Serial.begin(9600);

  pinMode(lightgatePin,  INPUT);
  pinMode(stirrerPin,    OUTPUT);
  pinMode(heaterPin,     OUTPUT);
  pinMode(thermistorPin, INPUT);
  pinMode(pHPin,         INPUT);
  attachInterrupt(digitalPinToInterrupt(lightgatePin),pulse,FALLING);

}

 
void loop() {
  heat();
  stir();
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

void stir(){
  double speed = get_speed();
  stir_error = get_difference(speed,setspeed);
  if (stir_error) != 0){
    get_output(setspeed);
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

double get_speed(){
  frequency = 0.5 / ((time - last_time)/60000);
  //pulseDetected = 0;
  Serial.print("motor speed--> ");
  Serial.print(frequency);
  Serial.print("   ");
  return frequency; 
}

void pulse(){
  //pulseDetected = 1;
  last_time = time;
  time = millis();
}

//------------------------------------------------------------------------------

double get_output(double setspeed){
  int val = map(setspeed, 500, 1500, 0, 255);
  analogWrite(stirrerPin, val);
  Serial.print("output = ");
  Serial.print(val);
}
