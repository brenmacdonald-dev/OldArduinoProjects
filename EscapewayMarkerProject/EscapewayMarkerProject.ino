#include <Servo.h>
#include <SoftwareSerial.h>
#define INPUT_SIZE 30
char input[INPUT_SIZE + 1];
int index = 0;
bool stringComplete = false;
bool HMFlagRaised = false;
int savedCommand = 0;
int savedParam = 0;
int opacity[10];
int sum = 0;
int opIndex = 0;

Servo Markers;
Servo Shutter;
Servo LifelineLeft;
Servo LifelineRight;

int blueLaserPin = 10;
int redLaserPin = 8;
int greenLaserPin = 9;
int capLampPin = 11;
int garageDoorPin = 12;

int transducerPin = 0;
int transducerValue = 0;
int transducerLowValue = 172;
int transducerHighValue = 725; //empirically measured @ lab
int smokeValue;

int priorMarkerMicroseconds = 900;
int targetMicroseconds = 0;

SoftwareSerial btSerial(50,51);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial1.begin(9600);//HM SERIAL!
  //btSerial.begin(9600);
  pinMode(blueLaserPin, OUTPUT);
  pinMode(redLaserPin, OUTPUT);
  pinMode(greenLaserPin, OUTPUT);
  pinMode(capLampPin, OUTPUT);
  pinMode(garageDoorPin, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  digitalWrite(garageDoorPin, HIGH);
  digitalWrite(blueLaserPin, 0);
  digitalWrite(redLaserPin, 0);
  digitalWrite(greenLaserPin, 0);
  Markers.attach(3);
  Markers.writeMicroseconds(900);
  Shutter.attach(4);
  Shutter.write(6);
  LifelineLeft.attach(5);
  LifelineRight.attach(6);
  //LifelineRight.write(0);
  LifelineRight.writeMicroseconds(900);
  LifelineLeft.writeMicroseconds(900);

  for(int i=0;i<10;i++){
    opacity[i]=0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(stringComplete){
    char* serparator = strchr(input, ',');
    if(serparator != 0){
      *serparator = 0;
      int command = atoi(input);
      ++serparator;
      int param = atoi(serparator);
      
      switch(command){
        case 0:
          Serial.println("Connected");
          break;
        case 1:
          break;
        case 2:          
          switch(param){
            case 0:
              //Markers.writeMicroseconds(900);
              //No Color
              priorMarkerMicroseconds = SmoothMoveToTarget(900);
              break;
            case 1:
              //Markers.writeMicroseconds(1050);
              //Supposed to be Green
              //priorMarkerMicroseconds = SmoothMoveToTarget(1050);
              priorMarkerMicroseconds = SmoothMoveToTarget(1404);
              break;
            case 2:
              //Markers.writeMicroseconds(1400);
              //Supposed to be Yellow
              //priorMarkerMicroseconds = SmoothMoveToTarget(1400);
              priorMarkerMicroseconds = SmoothMoveToTarget(1065);
              break;
            case 3:
              //Markers.writeMicroseconds(1700);
              //Supposed to be White
              //priorMarkerMicroseconds = SmoothMoveToTarget(1700);
              priorMarkerMicroseconds = SmoothMoveToTarget(1995);
              break;
            case 4:
              //Markers.writeMicroseconds(2000);
              //Supposed to be Yellow green
              //priorMarkerMicroseconds = SmoothMoveToTarget(2000);
              priorMarkerMicroseconds = SmoothMoveToTarget(1694);
              break;
          }
          Serial.print(command);
          Serial.print(',');
          Serial.println(param);
          break;
        case 3:
          if(param == 1){
            Shutter.write(60);
          }
          else{
            Shutter.write(6);
          }
          Serial.print(command);
          Serial.print(',');
          Serial.println(param);
          break;
        case 4:
          if(param == 0){
            digitalWrite(garageDoorPin, LOW);
            Serial.print(command);
            Serial.print(',');
            Serial.println(param);
            delay(1000);
            digitalWrite(garageDoorPin, HIGH);
          }
          if(param == 1){
            transducerValue = analogRead(transducerPin);
            if(transducerValue >= transducerHighValue){
              Serial.println("4,2");
            }
            else if(transducerValue <= transducerLowValue){
              Serial.println("4,1");
            }
            else{
              Serial.println("4,0");
            }
          }
          break;
        case 5:
          if(param == 0){
            //LifelineLeft.write(0);
            //LifelineRight.write(90);
            LifelineRight.writeMicroseconds(900);
            LifelineLeft.writeMicroseconds(900);
            Serial.print(command);
            Serial.print(',');
            Serial.println(param);
          }
          else{
            //LifelineLeft.write(90);
            //LifelineRight.write(0);
            //LifelineRight.writeMicroseconds(2100);
            //LifelineLeft.writeMicroseconds(2100);
            for(int i = 901;i<=2100;i++){
              LifelineRight.writeMicroseconds(i);
              LifelineLeft.writeMicroseconds(i);
              delay(3); 
            }
            Serial.print(command);
            Serial.print(',');
            Serial.println(param);
          }
          break;
        case 6:
          digitalWrite(redLaserPin,param);
          if(param == 1){
            btSerial.write('1');
            savedCommand = command;
            //savedParam = param;
            HMFlagRaised = true;
          }
          else{
            Serial.print(command);
            Serial.print(',');
            Serial.println(param);
            btSerial.write('0');
          }
          break;
        case 7:
          digitalWrite(greenLaserPin,param);
          if(param == 1){
            btSerial.write('2');
            savedCommand = command;
            HMFlagRaised = true;
          }
          else{
            btSerial.write('0');
            Serial.print(command);
            Serial.print(',');
            Serial.println(param);
          }          
          break;
        case 8:
          digitalWrite(blueLaserPin,param);
          if(param == 1){
            btSerial.write('3');
            savedCommand = command;
            HMFlagRaised = true;
          }
          else{
            btSerial.write('0');
            Serial.print(command);
            Serial.print(',');
            Serial.println(param);
          }          
          break;
        case 9:
          transducerValue = analogRead(A0);
          //Note 554 = 726 (closest point transducer val) - 172 (farthest point transducer val)
          //float transducerOutput;
          //transducerOutput = 185 - ((transducerValue-172)/554) * 85;
          //Serial.print(command);
          //Serial.print(',');
          Serial.println(transducerValue);
          break;
        case 10:
          digitalWrite(capLampPin, param);
          Serial.print(command);
          Serial.print(',');
          Serial.println(param);
          break;
        case 11:
          //smokeValue = analogRead(A1);
          int avg =0;          
          avg = sum/10;
          //Serial.print(command);
          //Serial.print(',');
          Serial.println(avg);
          break;
      }      
    }
    index = 0;    
    stringComplete = false;
    for(int i=0;i < INPUT_SIZE; i++){
      input[i] = ' ';    
    }
  }
  if(btSerial.available()){
    btSerial.read();
    if(HMFlagRaised){
      //Data received from the Versalume!      
      Serial.print(savedCommand);
      Serial.print(',');
      Serial.println(1);
      HMFlagRaised = false;
    }
  }
  sum = sum - opacity[opIndex];
  opacity[opIndex] = analogRead(A1);
  sum = sum + opacity[opIndex];
  opIndex++;
  if(opIndex >= 10){
    opIndex = 0;
  }
  /*
  sum -= opacity[0];
  for(int i=0;i<9;i++){
    opacity[i]=opacity[i+1];
  }
  opacity[9] = analogRead(A1);
  sum += opacity[9];
  */
}

void serialEvent(){
  while(Serial.available()){
    char inChar = (char)Serial.read();
    input[index] = inChar;
    index++;    
    if(inChar == '\n'){
      stringComplete = true;
    }
  }
}

int SmoothMoveToTarget(int target){
  int start = priorMarkerMicroseconds;
  bool isForward = target > start;
  int dTime = 5;
  int delta = 0;
  
  if(isForward){
    while(start <= target){
      Markers.writeMicroseconds(start);
      start++;
      if(target - start >= 1){//3
        dTime = 1;//7
      }
      else{
        dTime = 1;//5
      }
      delay(dTime);
    }
  }
  else{
    while(target <= start){
      Markers.writeMicroseconds(start);
      if(start - target <= 1){//3
        dTime = 1;//7
      }
      else{
        dTime = 1;//5
      }
      start--;
      delay(dTime);
    }
  }
  priorMarkerMicroseconds = target;  
  return priorMarkerMicroseconds;
}

