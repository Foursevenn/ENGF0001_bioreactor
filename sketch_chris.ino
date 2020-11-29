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
double get_heat_pid(void);
double get_stir_pid(void);
double get_temp(void);
double get_speed(void);
double get_settemp(void);
double get_setspeed(void);
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

double Kp_heat = 0;
double Ki_heat = 0;
double Kd_heat = 0;
double heat_error = 0;
double heat_error_pre = 0;
double heat_error_total = 0;

double Kp_stir = 0;
double Ki_stir = 0;
double Kd_stir = 0;
double stir_error = 0;
double stir_error_pre = 0;
double stir_error_total = 0;

void setup() {
  Serial.begin(9600);

  pinMode(lightgatePin,  INPUT);
  pinMode(stirrerPin,    OUTPUT);
  pinMode(heaterPin,     OUTPUT);
  pinMode(thermistorPin, INPUT);
  pinMode(pHPin,         INPUT);

  // More setup...
  
}

 
void loop() {
  heat();
  stir();
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
  return temp;
}

double get_speed(){
  double time1;
  double time2;
  double time;

  if (digitalRead(lightgatePin) == LOW){
    time1 = millis(); 
  }
  if (digitalRead(lightgatePin) == LOW){
    time2 = millis(); 
  }
  time = time2 - time1;
  double speed = 0.5/(time*1000*60); //rpm
  return speed;
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

//------------------------------------------------------------------------------

double get_output(double setspeed){
  int val = map(setspeed, 500, 1500, 0, 255);
  analogWrite(stirrerPin, val);
  Serial.print("output = ");
  Serial.print(val);
}
