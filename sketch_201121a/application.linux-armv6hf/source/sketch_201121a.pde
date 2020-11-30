import controlP5.*;
import processing.serial.*;
ControlP5 cp5;
Serial port;
PFont font;
double set_pH;
double set_heat;
double set_stir;
double pH;
double heat;
double stir;
String s;
PrintWriter output;
void setup(){
  
  font = createFont("Georgia", 20);
  
  cp5 = new ControlP5(this);
  printArray(Serial.list()); //?
  //port = new Serial(this,"COM3", 9600);   //dont undocument it when no serial connection with the sketch
  output = createWriter("log.txt");
  
  Group g1 = cp5.addGroup("Temperature")
                .setPosition(100,80)
                .setSize(300,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#7E8FB9))
                ;
  
  Group g2 = cp5.addGroup("Speed")
                .setPosition(500,80)
                .setSize(300,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#7E8FB9))
                ;
                
  Group g3 = cp5.addGroup("pH")
                .setPosition(100,360)
                .setSize(300,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#7E8FB9))
                ;
                
  Group g4 = cp5.addGroup("control_panel")
                .setPosition(500,360)
                .setSize(300,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#7E8FB9))
                ;
  
  cp5.addSlider("set_stir")
    .setLabel("Set rpm")
    .setPosition(20,160)
    .setSize(180,20)
    .setRange(0,1500)
    .setValue(800)
    .setFont(font)
    .setGroup(g2)
    ;
  cp5.addSlider("set_heat")
    .setLabel("Set °C")
    .setPosition(20,160)
    .setSize(180,20)
    .setRange(10,50)
    .setValue(30)
    .setFont(font)
    .setGroup(g1)
    ;
  cp5.addSlider("set_pH")
    .setLabel("Set mg/L")
    .setPosition(20,160)
    .setSize(180,20)
    .setRange(1,14)
    .setValue(5)
    .setFont(font)
    .setGroup(g3)
    ;
  cp5.addToggle("toggle")
     .setLabel("Master Switch")
     .setPosition(20,20)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     .setGroup(g4)
     ;
   
  //port = new Serial(this ,"/tmp/simavr-uart0",9600);
}

void settings(){
  size(900,620);
}

void draw(){
  
  background(200,200,200);
  
  fill(0,0,255);
  textFont(font);
  
  text("Bioreactor Team 50 V.1",340, 50);
  //output.println( + "t" + );
  
  /*if(port.available()>0){ 
    s = port.readStringUntil('\n');
  }*/
println(s);
}

void deliver(){
  stir = cp5.getController("set_stir").getValue();
  heat = cp5.getController("set_heat").getValue();
  pH = cp5.getController("set_pH").getValue();
  port.write("a"+stir+","+heat+","+pH+"\n");
}

void toggle(boolean theFlag){
  if(theFlag==true){
    println("aaaa");
  } else{
    println("bbbb");
  }
}

void keyPressed() {
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  exit(); // Stops the program
}
