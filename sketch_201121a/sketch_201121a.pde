import controlP5.*;
import processing.serial.*;
ControlP5 cp5;
Serial myport;
PFont font;
double set_pH;
double set_heat;
double set_stir;
double pH;
double heat;
double stir;
//String[] sensors;
//sensors = new String[3];
String[] sensors=new String[3];
String s;
PrintWriter output;
void setup(){
  
  font = createFont("Georgia", 20);
  
  cp5 = new ControlP5(this);
  printArray(Serial.list()); //?
  myport = new Serial(this,"/tmp/simavr-uart0", 9600);
// don’t generate a serialEvent() unless you get a newline character:
  myport.bufferUntil('\n');
  output = createWriter("log.txt");
  
  Group g1 = cp5.addGroup("Temperature")
                .setPosition(50,80)
                .setSize(375,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#F57D2F))
                ;
  
  Group g2 = cp5.addGroup("Speed")
                .setPosition(475,80)
                .setSize(375,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#1BDEEE))
                ;
                
  Group g3 = cp5.addGroup("pH")
                .setPosition(50,360)
                .setSize(375,200)
                //.setBackgroundHeight(200)
                .setBackgroundColor(color(#700ADD))
                ;
                
  
  cp5.addSlider("set_stir")
    .setLabel("Set rpm")
    .setPosition(20,160)
    .setSize(200,20)
    .setRange(0,1500)
    .setValue(800)
    .setFont(font)
    .setGroup(g2)
    ;
  cp5.addSlider("set_heat")
    .setLabel("Set °C")
    .setPosition(20,160)
    .setSize(200,20)
    .setRange(10,50)
    .setValue(30)
    .setFont(font)
    .setGroup(g1)
    ;
  cp5.addSlider("set_pH")
    .setLabel("Set pH")
    .setPosition(20,160)
    .setSize(200,20)
    .setRange(1,14)
    .setValue(5)
    .setFont(font)
    .setGroup(g3)
    ;
  cp5.addToggle("toggle")
     .setLabel("Master Switch")
     .setPosition(525,410)
     .setSize(275,100)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
  cp5.addChart("Temperature to time")
     .setPosition(20, 20)
     .setSize(200, 100)
     .setRange(20, 40)
     .setView(Chart.LINE) // use Chart.LINE, Chart.PIE, Chart.AREA, Chart.BAR_CENTERED
     .setStrokeWeight(1.5)
     .setColorCaptionLabel(color(40))
     .setGroup(g1)
     ;
  cp5.addChart("RPM to time")
     .setPosition(20, 20)
     .setSize(200, 100)
     .setRange(500, 1500)
     .setView(Chart.LINE) // use Chart.LINE, Chart.PIE, Chart.AREA, Chart.BAR_CENTERED
     .setStrokeWeight(1.5)
     .setColorCaptionLabel(color(40))
     .setGroup(g2)
     ;
  cp5.addChart("pH Value to time")
     .setPosition(20, 20)
     .setSize(200, 100)
     .setRange(1, 14)
     .setView(Chart.LINE) // use Chart.LINE, Chart.PIE, Chart.AREA, Chart.BAR_CENTERED
     .setStrokeWeight(1.5)
     .setColorCaptionLabel(color(40))
     .setGroup(g3)
     ;
  //port = new Serial(this ,"/tmp/simavr-uart0",9600);
}

void settings(){
  size(900,620);
}

void draw(){
  
  background(255,255,255);
  
  fill(150,0,255);
  textFont(font);
  text("Master Switch", 595, 380);
  text("Bioreactor Team 50 V.3",340, 50);
  //output.println( + "t" + );
  
  if(myport.available()>0){ 
    s = myport.readStringUntil('\n');
    s = trim(s);
    sensors[0] = split(s,",")[0];
    sensors[1] = split(s,",")[1];
    sensors[2] = split(s,",")[2];
    //for (int n =0; n<3; n++){
        //double sensors[n] = Double.parseDouble(sensor[n]);
    //}
    print("temperature--> " + sensors[0] + "\t");
    print("motor speed--> " + sensors[1] + "\t");
    println("pH--> " + sensors[2] );
    for (int n =0; n<3; n++){
        sensors[n] = null;
    }
  }
}

void deliver(){
  stir = cp5.getController("set_stir").getValue();
  heat = cp5.getController("set_heat").getValue();
  pH = cp5.getController("set_pH").getValue();
  myport.write("a"+stir+","+heat+","+pH+"\n");
}

void toggle(boolean theFlag){
  if(theFlag==true){
    myport.write("aaaa");
  } else{
    myport.write("bbbb");
  }
}

void keyPressed() {
  output.flush(); // Writes the remaining data to the file
  output.close(); // Finishes the file
  exit(); // Stops the program
}

//void serialEvent (Serial myPort) {
  // get the ASCII string:
//  String inString = myPort.readStringUntil('\n');
//  if (inString != null) {
    // trim off any whitespace:
//    inString = trim(inString);
//    println("original val.: ", inString);
//  }
//}
