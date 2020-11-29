/*
 sketch.ino

 Stephen Hailes, Oct 2020

 This is a template sketch for the testboard simduino
  
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

double stir_cali(void);
void pulse(void);
double heat_cali(void);
double pH_cali(void);
void output_data(void);

// These define which pins are connected to what device on the virtual bioreactor
//

const byte lightgatePin  = 2;
const byte stirrerPin    = 5;
const byte heaterPin     = 6;
const byte thermistorPin = A0;
const byte pHPin         = A1;

double last_time = 0;
double time = 0;
double frequency = 0.0;
const double total_vol = 5.0;
const double internal_R = 10000.0;
const double B = 4220.0;
const double R_25 = 10000.0;
const double t = 25.0+273.15;
int count = 0;

//volatile byte pulseDetected = 0;

// The PCA9685 is connected to the default I2C connections. There is no need
// to set these explicitly.


void setup() {
  Serial.begin(9600);

  pinMode(lightgatePin,  INPUT);
  pinMode(stirrerPin,    OUTPUT);
  pinMode(heaterPin,     OUTPUT);
  pinMode(thermistorPin, INPUT);
  pinMode(pHPin,         INPUT);
  attachInterrupt(digitalPinToInterrupt(lightgatePin),pulse,FALLING);
  // More setup...
  
}

 
void loop() {
  double s,t,pH;
  count += 1;
  analogWrite(stirrerPin,count*5);
  if(count == 1){analogWrite(heaterPin,200);}
  //analogWrite(stirrerPin,100);
  if(last_time !=0){
    t = heat_cali();
    Serial.print("temperature--> ");
    Serial.print(t);
    Serial.print("   ");
    s = stir_cali();
    Serial.print("motor speed--> ");
    Serial.print(s);
    Serial.print("   ");
    pH = pH_cali();
    Serial.print("pH--> ");
    Serial.print(pH);
    Serial.println("   ");
  }
  delay(30000);
}

double stir_cali(){
  frequency = 0.5 / ((time - last_time)/60000);
  //pulseDetected = 0;
  return frequency; 
}

void pulse(){
  //pulseDetected = 1;
  last_time = time;
  time = millis();
}

double heat_cali(){
  double analog_vol = analogRead(thermistorPin);
  double vol = (analog_vol/1023.0)*total_vol;
  double R = vol*internal_R/(total_vol-vol);
  double temp = (B*t)/(B+(log(R/R_25)*t));
  return temp;
}

double pH_cali(){
  double pH = analogRead(pHPin)/500.0 * 7.0;
  return pH;
}

  
  


 
  
