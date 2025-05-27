#include <Stepper.h>

#define STEPS 48
#define SPEED 150
#define STEPLIMIT 1000

Stepper stepper[6] = {Stepper(STEPS,8,9,10,11),Stepper(STEPS,22,23,24,25),Stepper(STEPS,26,27,28,29),Stepper(STEPS,30,31,32,33),Stepper(STEPS,34,35,36,37),Stepper(STEPS,38,39,40,41)};

int homePin = 42;
int ledWarning = 43;
boolean bFirstLoop = true;
boolean toggle = true;
boolean seekControl;
boolean seekAction;
boolean doCommand = false;
int controlCode = 0;
int actionCode = 0;

boolean motorActivated[6] = {false,false,false,false,false, false};

void setup(){
  for(int i=0; i<6; i++){
    stepper[i].setSpeed(SPEED);
  }
  pinMode(homePin, INPUT_PULLUP);
  pinMode(ledWarning, OUTPUT);  
  
  Serial.begin(9600);
  Serial.setTimeout(50);
}

void loop(){
  seekControl = true;
  seekAction = true;
  if(bFirstLoop){
    if(digitalRead(homePin) == LOW){
      Serial.println("Start");
      digitalWrite(ledWarning, LOW);
      bFirstLoop = false;
    }
    else{
      digitalWrite(ledWarning, HIGH);
    }
  }
  //ActivateMotors(toggle);
  while(Serial.available()>0){
    if(seekControl){
      controlCode = Serial.parseInt();
      seekControl = false;
    }
    if(seekAction){
      actionCode = Serial.parseInt();
      seekAction = false;
    }
    int remainingByte = Serial.read();
    doCommand = true;
  }
  if(doCommand){
    boolean motorDir = false;
    if(actionCode == 1){
      motorDir = true;
    }
    else{
      motorDir = false;
    }
    if(controlCode>=0 && controlCode<6){
      if(motorActivated[controlCode] != motorDir){
        ActivateSingleMotor(controlCode, motorDir);
      }
    }
    doCommand = false;
  }
  digitalWrite(ledWarning, LOW);
  //toggle = !toggle;
  //delay(500);
}

void ActivateMotors(boolean bToggle){
  digitalWrite(ledWarning, HIGH);
  int stepAmount = 48;
  for(int i=0; i<6; i++){
    Serial.print("Activate motor: ");
    Serial.println(i);
    if(!bToggle){
      stepAmount = -1*stepAmount;
    }
    stepper[i].step(stepAmount);
  }
}

void ActivateSingleMotor(int motor, boolean bToggle){
  digitalWrite(ledWarning, HIGH);
  int stepAmount = 48;
  Serial.print("Activate motor: ");
  Serial.print(motor);
  if(bToggle){
    Serial.println(", Up");
  }
  else{
    Serial.println(", Down");
    stepAmount = -1*stepAmount;
  }
  motorActivated[motor] = bToggle;
  stepper[motor].step(stepAmount);
}
