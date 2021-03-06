#include <stdio.h>
#include <string.h>
#include <Wire.h>
#include <stdbool.h>

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

double settemp = 300.0;
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
double filter_buf_t[FILTER_N] = {25.0,25.0,25.0,25.0,25.0,25.0,25.0,25.0,25.0,25.0};
int tArrayIndex =0;
double filter_buf_s[FILTER_N] = {1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0,1000.0};
int sArrayIndex =0;
double filter_buf_pH[FILTER_N] = {5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0,5.0};
int pHArrayIndex =0;
int count = 1;

String val1;
String c;
char* h = "Hello";
char* cc;
char* val2;
char* setv[3];
int connected = 0;
int handshake = 0;

// #define ArrayLength 10
// double pHArray[ArrayLength];
// int pHArrayIndex = 0;


void setup() {
  Serial.begin(9600);
 

  while (!Serial){
      ;
  }

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
//   establishContact();

}

 
void loop() {
  //if (connect != true){Serial.println("Hi");}
  if (Serial.available()>0&&connected == 0&&handshake==0) {
    c = Serial.readString();
    cc = c.c_str();
    //Serial.println(cc);
    if (strcmp(cc,h)==0&&handshake ==0){
      connected=1;
      Serial.println("Hi");
      handshake=1;
    }
  }
  if (connected == 1 && Serial.available()>0) {
      val1 = Serial.readString();
      val2 = val1.c_str();
      setv[0] = strtok(val2,",");
      setv[1] = strtok(NULL,",");
      setv[2] = strtok(NULL,",");
      settemp = atof(setv[0])+273.15;
      setspeed = atof(setv[1]);
      setpH = atof(setv[2]);
  }
  if (connected==1){
    heat();
    stir();
    pH();
    count+=1;
    if(count == 10) {count = 0;}
    delay(100);
  }
}

// void establishContact(){
//     while (Serial.available() <= 0){
//         Serial.println("0,0,0");
//         delay(300);
//     }
//}

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

double average_array(double* arr, int number){
  int i;
  double max,min;
  double avg;
  double amount = 0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
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
    analogWrite(heaterPin, 60);
  } else{
    analogWrite(heaterPin, 0);
  }
  if(count == FILTER_N-1){
    // tArrayIndex = 0;
    filtered_t= average_array(filter_buf_t,FILTER_N);
    // Serial.print("temperature--> ");
    Serial.print(filtered_t-273.15);
    Serial.print(',');
    // Serial.print("   ");
    // Serial.print("temp_error--> ");
    // Serial.print(get_difference(filtered_t,settemp));
    // Serial.println();
    clear_buf(filter_buf_t); 
  }
}

void stir(){
  double filtered_s;
  double speed = get_speed();
  filter_buf_s[count] = speed; 
  stir_error = get_difference(speed,setspeed+10);
  if (stir_error != 0){
    get_output();
  }
  if(count == FILTER_N-1){
    // sArrayIndex = 0;
    filtered_s= average_array(filter_buf_s,FILTER_N);
    // Serial.print("motor speed--> ");
    Serial.print(filtered_s);
    Serial.print(',');
    // Serial.print("   ");
    // Serial.print("speed_error--> ");
    // Serial.print(get_difference(filtered_s,setspeed));
    // Serial.println();
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
  if(count == FILTER_N-1){
  //analogWrite(stirrerPin,100);
    sArrayIndex = 0;
    filtered_pH= average_array(filter_buf_pH,FILTER_N);
    // Serial.print("pH--> ");
    Serial.println(filtered_pH);
    // Serial.print("   ");
    // Serial.print("pH_error--> ");
    // Serial.print(get_difference(filtered_pH,setpH));
    // Serial.println();
    // Serial.println();
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

double get_difference(double value,double setvalue){
  return setvalue - value;
}

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
  frequency = 0.5 / ((time - last_time)/60000.0);
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

double get_output(){
  double val = map(setspeed, 500.0, 1500.0, 41.5, 122.5);
  analogWrite(stirrerPin, val);
}
