#include <Servo.h> 

#define DEBUGGING 0
#define DELAY_DEBUG 000

//Define how many pots and it's positions (angles)
#define POTS 8

//Defining servo properties
#define SERVO_PIN 6
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
#define G_pin 9
#define B_pin 11

// Stepper pins and properties
#define END_STOP_PIN 8
#define BEGIN_OFFSET 0
#define DIRECTION_PIN 12
#define STEP 13
#define STEP_DELAY 1
#define CW 0
#define CCW 1


Servo myservo; 
int angle;
int cur_steps;
int new_angle=0;
unsigned long R,G,B;
String rgbstring;

int sorted=0;
/*
class Color{
  public
}*/

void setup() { 
  Serial.begin(57600);
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
    /*rgb = "RGB:\t";
    rgb += String(R) + "\t" ;
    rgb += String(G) + "\t" ;
    rgb += String(B) + "\t" ;
    Serial.print(rgb);
    delay(100);*/
    if (R < 29 && R > 13 && G < 29 && G > 14 && B < 30 && B > 18){
      #if DEBUGGING==1
         Serial.print("VERDE");
      #endif
      cores[0]++;
    }

    else if (R < 61 && R > 31 && G < 49 && G > 30 && B < 20 && B > 14){
      #if DEBUGGING==1
      Serial.print("AZUL MARINHO");
      #endif
      cores[1]++;
    }

    else if (R < 45 && R > 30 && G < 44 && G > 24 && B < 24 && B > 11){
      #if DEBUGGING==1
      Serial.print("AZUL CELESTE");
      #endif
      cores[2]++;
    }

    else if (R < 29 && R > 7 && G < 32 && G > 5 && B < 28 && B > 6){
      #if DEBUGGING==1
      Serial.print("VERDE TRANSPARENTE");
      #endif
      cores[3]++;
    }

    else if (R < 63 && R > 39 && G < 41 && G > 27 && B < 61 && B > 40){
      #if DEBUGGING==1
      Serial.print("ESMERALDA");
      #endif
      cores[4]++;
    }

    else  if (R < 37 && R > 21 && G < 49 && G > 23 && B < 79 && B > 35){
      #if DEBUGGING==1
      Serial.print("VERDE AMARELADO");
      #endif
      cores[5]++;
    }

    else if (R < 63 && R > 28 && G < 80 && G > 49 && B < 74 && B > 45){
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
    setRGBa(0,255,50,100);
    setStep(25);
    break;
  case 1:
    Serial.print("AZUL MARINHO");
    setRGBa(10,10,255,100);
    setStep(50);
    break;
  case 2:
    Serial.print("AZUL CELESTE");
    setRGBa(140,250,255,100);
    setStep(75);
    break;
  case 3:
    Serial.print("VERDE TRANSPARENTE");
    setRGBa(180,255,150,100);
    setStep(100);
    break;
  case 4:
    Serial.print("ESMERALDA");
    setRGBa(0,255,20,100);
    setStep(125);
    break;
  case 5:
    Serial.print("VERDE AMARELADO");
    setRGBa(130,180,0,100);
    setStep(150);
    break;
  case 6:
    Serial.print("GASOLINA");
    setRGBa(255,128,0,100);
    setStep(175);
    break;
  case 7:
    Serial.print("NAO DEFINIDA");
    setRGBa(0,0,0,0);
    setStep(0);
    break;
  case 8:
    Serial.print("NENHUMA BOLINHA");
    setRGBa(100,100,0,100);
    break;
  }

  Serial.println(" \n");
  delay(50);
}
int biggestIntIndex(short int values[]){
  int biggestIntIndex = 8;
  int mysize = POTS;
  for(int i=0;i<mysize;i++){
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
      delay(100);
      findBeginning();
      sorted=0;
    }
    myservo.write(LET_IN_ANGLE);
    delay(LET_IN_TIME);
    delay(DELAY_DEBUG);

    // Check color and move the Stepper
    myservo.write(PHOTO_ANGLE);

    whichColorIsIt();
    //delay(PHOTO_TIME/2);
    delay(DELAY_DEBUG);

    myservo.write(LET_OUT_ANGLE);
    delay(LET_OUT_TIME);
    delay(DELAY_DEBUG);
  }
  //delay(100);
} 

