/*
 sketch.ino

 Stephen Hailes, Oct 2020

 This is a template sketch for the testboard simduino
  
 */

#include <stdio.h>
#include <string.h>
#include <Wire.h>
void pH(void);
double get_pH(void);
double get_setvalue(void);
// double,double,double get_PID(void);
double get_difference(double,double);
// double get_PID_value(double,double,double,double);
double val1;
double val2;
double val3;
double pHnow;
// These define which pins are connected to what device on the virtual bioreactor
//
const byte lightgatePin  = 2;
const byte stirrerPin    = 5;
const byte heaterPin     = 6;
const byte thermistorPin = A0;
const byte pHPin         = A1;

// The PCA9685 is connected to the default I2C connections. There is no need
// to set these explicitly.


void setup() {
  
  Wire.begin();
  Serial.begin(9600);

  pinMode(lightgatePin,  INPUT);
  pinMode(stirrerPin,    OUTPUT);
  pinMode(heaterPin,     OUTPUT);
  pinMode(thermistorPin, INPUT);
  pinMode(pHPin,         INPUT);
  
  // More setup...
  
}


 
void loop() {
  double setV=get_setvalue();
  double nowV=get_pH();
  Serial.print(nowV);
  double error = get_difference(nowV,setV);
  if (true){
     Wire.beginTransmission(0x40);
     Wire.write(0x06);
     Wire.write(0x99);
     Wire.write(0x01);
     Wire.write(0xCC);
     Wire.write(0x04);
     Wire.endTransmission();
  }
  /*else if(error<-0.5){
    Wire.beginTransmission(0x40);
    Wire.write(0x0B);
    Wire.write(0x0A);
    delay(5);
    Wire.write(0x0D);
  }
  else{
    delay(5);
  }
  nowV = get_pH();
  Serial.print("output = ");
  Serial.print(nowV);
  delay(50);*/
  delay(5000);
}


double get_setvalue(void){
  double valDefault = 5;
  /*if(Serial.available()){
    if(Serial.find("a"))  {
    val1=Serial.parseInt();
    val2=Serial.parseInt();
    val3=Serial.parseInt();
    return val3;
  }
  }*/
  return valDefault;
}

double get_pH(){
  pHnow = analogRead(pHPin);
  pHnow = pHnow/500.0*7;
  return pHnow;
}

double get_difference(double now,double tar){
  double error = now - tar;
  return error;
}
