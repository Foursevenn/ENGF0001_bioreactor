

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Wire.h>

double stir_cali(void);
void pulse(void);
double heat_cali(void);
double pH_cali(void);
void output_data(void);
double filter();
double filter_buf_t[FILTER_N];
double filter_buf_s[FILTER_N];
double filter_buf_pH[FILTER_N];

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
int count = 1;
#define FILTER_N 10

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
  Wire.beginTransmission(0x40);
  Wire.write(0x00);
  Wire.write(0x21);
  Wire.endTransmission();
  // More setup...
  
}

 
void loop() {
  double s,t,pH;
  int i;
  double filtered_t,filtered_s,filtered_pH;
  if(count == 1){analogWrite(heaterPin,200);}
  analogWrite(stirrerPin,count*0.5);
  Wire.beginTransmission(0x40);
  Wire.write(0x06);
  Wire.write(0x01);
  Wire.write(0x00);
  Wire.write(0xFF);
  Wire.write(0xEF);
  Wire.endTransmission();
  t = heat_cali();
  s = stir_cali();
  pH = pH_cali();
  filter_buf_t[count] = t; 
  filter_buf_s[count] = s; 
  filter_buf_pH[count] = pH; 
  count += 1;
  if(last_time !=0 && count %FILTER_N == 0){
  //analogWrite(stirrerPin,100);
    filtered_t= filter(filter_buf_t);
    filtered_s= filter(filter_buf_s);
    filtered_pH= filter(filter_buf_pH);
    Serial.print("temperature--> ");
    Serial.print(filtered_t);
    Serial.print("   ");
    Serial.print("motor speed--> ");
    Serial.print(filtered_s);
    Serial.print("   ");
    Serial.print("pH--> ");
    Serial.print(filtered_pH);
    Serial.println("   ");
    clear_buf(filtered_t); 
    clear_buf(filtered_s); 
    clear_buf(filtered_pH); 
    count = 0;
  }
  delay(3000);
}

double clear_buf(double buf){
  int i;
  for(i = 0;i<FILTER_N;i++){
    a[i] = 0.0;
  }
}

double filter(double buf){
  int i,j;
  double filter1, filter_sum = 0;
  for (j = 0; j < FILTER_N; j++){
    for (i = 0; i < FILTER_N - j; i++){
      if(buf[i]> buf[i+1]){

      filter1 = buf[i];
      buf[i] = nuf[i+1];
      buf[i+1] = filter1;
      }
    }
  }
  for(i = 1; i < FILTER_N-1;i++){
    filter_sum += buf[i];
  }
  return filter_sum / (FILTER_N-2);
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

  
  


 
  
