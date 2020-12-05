/*
 sketch.ino

 Stephen Hailes, Oct 2020

 This is a template sketch for the testboard simduino
  
 */

#include <stdio.h>
#include <string.h>

void heat(void);
void stir(void);
void pH(void);
double get_difference(double value,double setvalue);
double get_heat_pid(void);
double get_stir_pid(void);
double get_temp(void);
double get_speed(void);
double get_pH(void);
double get_settemp(void);
double get_setspeed(void);
double get_setpH(void);
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

double Kp_heat = 1;
double Ki_heat = 1;
double Kd_heat = 1;
double heat_error = 0;
double heat_error_pre = 0;
double heat_error_total = 0;

double Kp_stir = 1;
double Ki_stir = 1;
double Kd_stir = 1;
double stir_error = 0;
double stir_error_pre = 0;
double stir_error_total = 0;

double last_time = 0;
double time = 0;
double frequency = 0.0;
const double total_vol = 5.0;
const double internal_R = 10000.0;
const double B = 4220.0;
const double R_25 = 10000.0;
const double t = 25.0+273.15;
int count = 0;
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
  ph();
  delay(5000);
}

//------------------------------------------------------------------------------

void heat(){
  double temp = get_temp();
  double settemp = get_settemp();
  heat_error = get_difference(temp,settemp);
  heat_error_total += heat_error;
  double heat_pid = get_heat_pid();
  if (heat_pid > 0){
    digitalWrite(heaterPin, HIGH);
  }
  if (heat_pid <= 0){
    digitalWrite(heaterPin, LOW);
  }
  heat_error_pre = heat_error;
  heat_error = 0;
}

void stir(){
  double speed = get_speed();
  double setspeed = get_setspeed();
  stir_error = get_difference(speed,setspeed);
  double stir_pid = get_stir_pid();
  if (stir_pid) != 0){
    get_output(setspeed);
  }
  stir_error_pre = stir_error;
  stir_error = 0;
}

void pH(){
  double setV=get_setvalue();
  double nowV=get_pH();
  double error = get_difference(nowV,setV);
  if (error<-0.5){
     Wire.beginTransmission(0x40);
     Wire.write(0x07);
     Wire.write(0x06);
     delay(5);
     Wire.write(0x09);
  }
  else if(error>0.5){
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
  delay(50);
}

//------------------------------------------------------------------------------

double get_difference(double value,double setvalue){
  return setvalue - value;
}

double get_heat_pid(){
  heat_pid = Kp_heat * heat_error + Ki_heat * heat_error_total + Kd_heat * (heat_error - heat_error_pre);
  return heat_pid;
}

double get_stir_pid(){
  stir_pid = Kp_stir * stir_error + Ki_stir * stir_error_total + Kd_stir * (stir_error - stir_error_pre);
  return stir_pid;
}

//------------------------------------------------------------------------------

double get_temp(){
  double analog_vol = analogRead(thermistorPin);
  double vol = (analog_vol/1023)*total_vol;
  double R = vol*internal_R/(total_vol-vol);
  double temp = (B*t)/(B-(log(R/R_25)*t)) + 273.15;
  Serial.print("temperature--> ");
  Serial.print(temp);
  Serial.print("   ");
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

double get_pH(){
  pHnow = analogRead(pHPin);
  pHnow = pHnow/500.0*7;
  Serial.print("pH--> ");
  Serial.print(pH);
  Serial.println("   ");
  return pHnow;
}

//------------------------------------------------------------------------------

double get_settemp(){
  double settemp;
  printf("Please enter the teemperature for the system: ");
  scanf("%lf",&settemp);
  return settemp;
}

double get_setspeed(){
  double setspeed;
  printf("Please enter the speed for stirrer: ");
  scanf("%lf",&setspeed);
  return setspeed;
}

double get_setpH(void){
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

//------------------------------------------------------------------------------

double get_output(double setspeed){
  int val = map(setspeed, 500, 1500, 0, 255);
  analogWrite(stirrerPin, val);
  Serial.print("output = ");
  Serial.print(val);
}
