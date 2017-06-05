///////////////////////////////////////////////////
/////////////  PIN MAPS ///////////////////////////
///////////////////////////////////////////////////

// 0 -> OUTPUT -> TCS230 (S0)
// 1 -> OUTPUT -> TCS230 (S1)
// 2 -> OUTPUT -> TCS230 (S2)
// 3 -> OUTPUT -> TCS230 (S3)
// 4 -> INPUT -> TCS230 (sensorOut)
// 5 -> 
// 6 -> 
// 7 -> BT Rx
// 8 -> BT Tx
// 9 -> 
// 10 -> OUTPUT -> Servo
// 11 -> INPUT -> End Stop
// 12 -> OUTPUT -> Direction
// 13 -> OUTPUT -> Step

///////////////////////////////////////////////////
/////////////  END PIN MAPS ///////////////////////
///////////////////////////////////////////////////

#include <SoftwareSerial.h>
#include <Wire.h>
#include <Servo.h> 
SoftwareSerial BT(7,8);

#define DEBUGGING 0
#define DELAY_DEBUG 000

//Define how many pots and it's positions (angles)
#define POTS 8

//Defining servo properties
#define SERVO_PIN 10
#define LET_OUT_ANGLE 38
#define PHOTO_ANGLE 96
#define LET_IN_ANGLE 155
#define LET_OUT_TIME 310
#define LET_IN_TIME 300
#define PHOTO_TIME 100

// Marble Detector
#define LDR_DETECTOR_PIN A0
#define LDR_PRESENCE_THRESHOLD 680

// Color Sensor
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define sensorOut 4
#define EACH_COLOR_TIME 5

// RGB led pins
#define R_pin 5
#define G_pin 6
#define B_pin 9

// Stepper pins and properties
#define END_STOP_PIN 11
#define BEGIN_OFFSET 0
#define DIRECTION_PIN 12
#define STEP 13
#define STEP_DELAY 1
#define CW 1
#define CCW 0


Servo myservo; 
int angle;
int cur_steps;
int new_angle=0;
unsigned long R,G,B;
String rgbstring;
int counters[]={0,0,0,0,0,0,0,0,0};
int sorted=0;
/*
class Color{
  public
}*/

void setup() { 
  Serial.begin(57600);
  BT.begin(9600);
  BT.println("*LR0G0B0**A0**B0**C0**D0**E0**F0**G0**H0**I0*");
  myservo.attach(SERVO_PIN);
  myservo.write(PHOTO_ANGLE);

  pinMode(LDR_DETECTOR_PIN, INPUT);
  pinMode(END_STOP_PIN, INPUT);
  pinMode(DIRECTION_PIN, OUTPUT);
  digitalWrite(DIRECTION_PIN, CW);
  pinMode(STEP, OUTPUT);

  //Color Sensor and RGB led Initialization
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(R_pin, OUTPUT);
  pinMode(G_pin, OUTPUT);
  pinMode(B_pin, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  rgbstring.reserve(16);

  findBeginning();
  delay(5000);

} 
boolean isPresent(){
  int val = analogRead(LDR_DETECTOR_PIN);
  if(val>LDR_PRESENCE_THRESHOLD) return false;
  else return true;
}
void findBeginning(){
  while(digitalRead(END_STOP_PIN)==LOW) {
    digitalWrite(STEP,HIGH);
    delay(STEP_DELAY);
    digitalWrite(STEP,LOW);
    delay(STEP_DELAY);
  }
  cur_steps=0;
  angle=0;
  Serial.println("END REACHED");
}


void setStep(int newstep){
  int steps = newstep - cur_steps;
  #if DEBUGGING==1
  Serial.print("Cur Steps: ");
   Serial.println(cur_steps);
   
   Serial.print("New Step: ");
   Serial.println(newstep);
   
   Serial.print("Steps: ");
   Serial.println(steps);
   
   
  #endif
  if (steps<=100 && steps>0){
    digitalWrite(DIRECTION_PIN,CW);
  } 

  else if(steps>100){
    digitalWrite(DIRECTION_PIN,CCW);
    steps=200-steps;
  }

  else if(steps<0 && steps>-100){
    digitalWrite(DIRECTION_PIN,CCW);
    steps=abs(steps);
  }
  else if(steps<=-100){
    digitalWrite(DIRECTION_PIN,CW);
    steps=200-abs(steps);  
  }

  for(int i=0;i<steps;i++){
    digitalWrite(STEP,HIGH);
    delay(STEP_DELAY);
    digitalWrite(STEP,LOW);
    delay(STEP_DELAY);
  }
  if(newstep==200) newstep=0;
  else cur_steps=newstep;

  #if DEBUGGING==1
   Serial.print("Cur Steps: ");
   Serial.println(cur_steps);
   
   Serial.print("New Step: ");
   Serial.println(newstep);
   
   Serial.print("Steps: ");
   Serial.println(steps);
   
   
  #endif

}
void setRGBa(int R_val, int G_val, int B_val, int a){
  int maximo = map(a,0,100,0,255);
  R_val=map(R_val,0,255,0,maximo);
  G_val=map(G_val,0,255,0,maximo);
  B_val=map(B_val,0,255,0,maximo);
  analogWrite(R_pin,R_val);
  analogWrite(G_pin,G_val);
  analogWrite(B_pin,B_val);
  
  #if DEBUGGING==1
  Serial.println("RGB R: ");
  Serial.println(R_val);
  Serial.println(" G: ");
  Serial.println(G_val);
  Serial.println("RGB B: ");
  Serial.println(B_val);
  #endif

}
unsigned long getRed(){
  unsigned long aux;
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  aux = pulseIn(sensorOut,LOW);
  delay(EACH_COLOR_TIME);
  return aux;
}
unsigned long getGreen(){
  unsigned long aux;
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  aux = pulseIn(sensorOut,LOW);
  delay(EACH_COLOR_TIME);
  return aux;
}
unsigned long getBlue(){
  unsigned long aux;
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  aux = pulseIn(sensorOut,LOW);
  delay(EACH_COLOR_TIME);
  return aux;
}
void whichColorIsIt(){
  short int cores[POTS+1];
  for(int j=0;j<POTS+1;j++) cores[j] = 0;
  for(int i = 0; i < 15; i++){
    R = getRed();
    G = getGreen();
    B = getBlue();
    /*rgbstring = "RGB:\t";
    rgbstring += String(R) + "\t" ;
    rgbstring += String(G) + "\t" ;
    rgbstring += String(B) + "\t" ;
    Serial.println(rgbstring);
    delay(100);*/
    if (R < 24 && R > 11 && G < 29 && G > 14 && B < 33 && B > 17){
      #if DEBUGGING==1
         Serial.print("VERDE");
      #endif
      cores[0]++;
    }

    else if (R < 51 && R > 21 && G < 49 && G > 30 && B < 20 && B > 11){
      #if DEBUGGING==1
      Serial.print("AZUL MARINHO");
      #endif
      cores[1]++;
    }

    else if (R < 46 && R > 30 && G < 37 && G > 21 && B < 19 && B > 9){
      #if DEBUGGING==1
      Serial.print("AZUL CELESTE");
      #endif
      cores[2]++;
    }

    else if (R < 19 && R > 5 && G < 20 && G > 6 && B < 16 && B > 5){
      #if DEBUGGING==1
      Serial.print("VERDE TRANSPARENTE");
      #endif
      cores[3]++;
    }

    else if (R < 62 && R > 32 && G < 42 && G > 22 && B < 61 && B > 30){
      #if DEBUGGING==1
      Serial.print("ESMERALDA");
      #endif
      cores[4]++;
    }

    else  if (R < 40 && R > 15 && G < 53 && G > 14 && B < 67 && B > 30){
      #if DEBUGGING==1
      Serial.print("VERDE AMARELADO");
      #endif
      cores[5]++;
    }

    else if (R < 66 && R > 18 && G < 87 && G > 32 && B < 80 && B > 38){
      #if DEBUGGING==1
      Serial.print("GASOLINA");
      #endif
      cores[6]++;
    } 


    else if (R < 4 && R > 0 && G < 4 && G > 0 && B < 4 && B > 0){
      #if DEBUGGING==1
      Serial.print("NENHUMA BOLINHA");
      #endif
      cores[8]++;

    }

    else{
      #if DEBUGGING==1
      Serial.print("NAO DEFINIDA");
      #endif
      cores[7]++;

    }
    #if DEBUGGING==1
    Serial.println(" ");
    delay(100);
    #endif
    delay(50);
    
  }
  int decision = biggestIntIndex(cores);
  Serial.print("FINAL DECISION: ");
  //Serial.print(decision);
  switch(decision){
  case 0:
    Serial.print("VERDE");
    BT.println("*LR0G255B50*");
    
    setRGBa(0,255,50,100);
    counters[0]++;
    BT.println("*B"+String(counters[0])+"*");
    setStep(25);
    break;
  case 1:
    Serial.print("AZUL MARINHO");
    BT.println("*LR10G10B255*");
    setRGBa(10,10,255,100);
    counters[1]++;
    BT.println("*C"+String(counters[1])+"*");
    setStep(50);
    break;
  case 2:
    Serial.print("AZUL CELESTE");
    BT.println("*LR140G250B255*");
    setRGBa(140,250,255,100);
    counters[2]++;
    BT.println("*D"+String(counters[2])+"*");
    setStep(75);
    break;
  case 3:
    Serial.print("VERDE TRANSPARENTE");
    BT.println("*LR180G255B150*");
    setRGBa(180,255,150,100);
    counters[3]++;
    BT.println("*E"+String(counters[3])+"*");
    setStep(100);
    break;
  case 4:
    Serial.print("ESMERALDA");
    BT.println("*LR0G255B20*");
    setRGBa(0,255,20,100);
    counters[4]++;
    BT.println("*F"+String(counters[4])+"*");
    setStep(125);
    break;
  case 5:
    Serial.print("VERDE AMARELADO");
    BT.println("*LR130G180B0*");
    setRGBa(130,180,0,100);
    counters[5]++;
    BT.println("*G"+String(counters[5])+"*");
    setStep(150);
    break;
  case 6:
    Serial.print("GASOLINA");
    BT.println("*LR255G128B0*");
    setRGBa(255,128,0,100);
    counters[6]++;
    BT.println("*H"+String(counters[6])+"*");
    setStep(175);
    break;
  case 7:
    Serial.print("NAO DEFINIDA");

    BT.println("*LR255G0B0*");
    BT.println("*S100*");
    setRGBa(0,0,0,0);
    counters[7]++;
    BT.println("*A"+String(counters[7])+"*");
    setStep(0);
    break;
  case 8:
    Serial.print("NENHUMA BOLINHA");
    BT.println("*LR255G255B255*");
    BT.println("*S50*");
    counters[8]++;
    BT.println("*I"+String(counters[8])+"*");
    setRGBa(100,100,0,100);
    break;
    
  }

  Serial.println(" \n");
  delay(50);
}

int biggestIntIndex(short int values[]){
  int biggestIntIndex = POTS;
  for(int i=0;i<POTS;i++){
    if (values[i]>values[biggestIntIndex])
      biggestIntIndex = i;
  }
  return biggestIntIndex;
}
void loop() 
{ 
  if(isPresent()){
    sorted++;
    if(sorted==10){
      // Let the marble out first, then go to the beginning
      delay(200);
      findBeginning();
      sorted=0;
    }
    myservo.write(LET_IN_ANGLE);
    delay(LET_IN_TIME);
    delay(DELAY_DEBUG);

    // Check color and move the Stepper
    myservo.write(PHOTO_ANGLE);
    //delay(300);
    whichColorIsIt();
    
    //delay(PHOTO_TIME/2);
    delay(DELAY_DEBUG);

    myservo.write(LET_OUT_ANGLE);
    delay(LET_OUT_TIME);
    delay(DELAY_DEBUG);
  }

  //Serial.println(analogRead(LDR_DETECTOR_PIN));
} 


