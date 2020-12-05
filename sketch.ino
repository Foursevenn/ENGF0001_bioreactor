/*
 sketch.ino

 Stephen Hailes, Oct 2020

 This is a template sketch for the testboard simduino
  
 */

#include <stdio.h>
#include <string.h>
#include <Wire.h>

void heat(void);
void stir(void);
void pH(void);
double get_difference(double value,double setvalue);
double get_temp(void);
double get_speed(void);
double get_pH(void);
double get_output(void);
double filter(double);
void clear_buf(double buf[]);

// These define which pins are connected to what device on the virtual bioreactor
//
const byte lightgatePin  = 2;
const byte stirrerPin    = 5;
const byte heaterPin     = 6;
const byte thermistorPin = A0;
const byte pHPin         = A1;

// The PCA9685 is connected to the default I2C connections. There is no need
// to set these explicitly.

double settemp = 299.0-1.5;
double heat_error = 0.0;

double setspeed = 1000;
double stir_error = 0.0;
double frequency = 0.0;

double setpH = 5.0;
double pH_error = 0.0;
double pHnow = 0.0;

double last_time = 0;
double time = 0;
const double total_vol = 5.0;
const double internal_R = 10000.0;
const double B = 4220.0;
const double R_25 = 10000.0;
const double t = 25.0+273.15;

#define FILTER_N 10
double filter();
double filter_buf_t[FILTER_N];
double filter_buf_s[FILTER_N];
double filter_buf_pH[FILTER_N];
int count = 1;

void setup() {
  Serial.begin(9600);

  pinMode(lightgatePin,  INPUT);
  pinMode(stirrerPin,    OUTPUT);
  pinMode(heaterPin,     OUTPUT);
  pinMode(thermistorPin, INPUT);
  pinMode(pHPin,         INPUT);
  attachInterrupt(digitalPinToInterrupt(lightgatePin),pulse,FALLING);
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.write(0x21);
  Wire.endTransmission();

}

 
void loop() {
  heat();
  stir();
  pH();
  count+=1;
  if(count == 10) {count = 0;}
  delay(3000);
}

//------------------------------------------------------------------------------
double filter(double buf[]){
  int i,j;
  double filter1, filter_sum = 0;
  for (j = 0; j < FILTER_N; j++){
    for (i = 0; i < FILTER_N - j; i++){
      if(buf[i]> buf[i+1]){

      filter1 = buf[i];
      buf[i] = buf[i+1];
      buf[i+1] = filter1;
      }
    }
  }
  for(i = 1; i < FILTER_N-1;i++){
    filter_sum += buf[i];
  }
  return filter_sum / (FILTER_N-2);
}

void clear_buf(double buf[]){
  int i;
  for(i = 0;i<FILTER_N;i++){
    buf[i] = 0.0;
  }
}

void heat(){
  double filtered_t;
  double temp = get_temp();
  filter_buf_t[count] = temp; 
  heat_error = get_difference(temp,settemp);
  if (heat_error > 5.0){
    analogWrite(heaterPin, 255);
  } else if (heat_error > 3.0){
    analogWrite(heaterPin, 175);
  } else if (heat_error > 1.0){
    analogWrite(heaterPin, 125);
  } else if (heat_error > 0.5){
    analogWrite(heaterPin, 70);
  } else if (heat_error > 0.3){
    analogWrite(heaterPin, 30);
  } else{
    analogWrite(heaterPin, 0);
  }
  if(count == 9){
  //analogWrite(stirrerPin,100);
    filtered_t= filter(filter_buf_t);
    Serial.print("temperature--> ");
    Serial.print(filtered_t);
    Serial.print("   ");
    Serial.print("temp_error--> ");
    Serial.print(heat_error);
    Serial.println();
    clear_buf(filter_buf_t); 
  }
}

void stir(){
  double filtered_s;
  double speed = get_speed();
  filter_buf_s[count] = speed; 
  stir_error = get_difference(speed,setspeed);
  if (stir_error != 0){
    get_output();
  }
  if(count == 9){
  //analogWrite(stirrerPin,100);
    filtered_s= filter(filter_buf_s);
    Serial.print("motor speed--> ");
    Serial.print(filtered_s);
    Serial.print("   ");
    Serial.print("speed_error--> ");
    Serial.print(stir_error);
    Serial.println();
    clear_buf(filter_buf_s); 
  }
}

void pH(){
  double filtered_pH;
  // double setV = setpH;
  double nowpH = get_pH();
  filter_buf_pH[count] = nowpH;
  pH_error = get_difference(nowpH,setpH);
  if (pH_error<-0.2){
     Wire.beginTransmission(0x40);
     Wire.write(0x06);
     Wire.write(0x01);
     Wire.write(0x00);
     Wire.write(0xFF);
     Wire.write(0x0F);
     Wire.endTransmission();
  }
  else if(pH_error>0.2){
     Wire.beginTransmission(0x40);
     Wire.write(0x0A);
     Wire.write(0x01);
     Wire.write(0x00);
     Wire.write(0xFF);
     Wire.write(0x0F);
     Wire.endTransmission();
  }
  if(count == 9){
  //analogWrite(stirrerPin,100);
    filtered_pH= filter(filter_buf_pH);
    Serial.print("pH--> ");
    Serial.print(filtered_pH);
    Serial.print("   ");
    Serial.print("pH_error--> ");
    Serial.print(pH_error);
    Serial.println();
    Serial.println();
    clear_buf(filter_buf_pH); 
  }
  // else{
  //   delay(5);
  // }
  // nowV = get_pH();
  // Serial.print("pH--> ");
  // Serial.print(nowV);
  // Serial.print("   ");
  // delay(50);
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
  // Serial.print("temperature--> ");
  // Serial.print(temp);
  // Serial.print("   ");
  return temp;
}

double get_speed(){
  frequency = 0.5 / ((time - last_time)/60000);
  //pulseDetected = 0;
  // Serial.print("motor speed--> ");
  // Serial.print(frequency);
  // Serial.print("   ");
  return frequency; 
}

double get_pH(){
  pHnow = analogRead(pHPin);
  pHnow = (pHnow-(300.0 - 200.0/3.0*4.0))/(200.0/3.0);
  return pHnow;
}

void pulse(){
  //pulseDetected = 1;
  last_time = time;
  time = millis();
}

//------------------------------------------------------------------------------

double get_output(){
  int val = map(setspeed, 500, 1500, 41.5, 122.5);
  analogWrite(stirrerPin, val);
}
