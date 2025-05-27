#include <Stepper.h>

//Number of steps per revolution in the motors I have
#define STEPS 48
#define SPEED 150 //tested as a good rpm
//#define STEPLIMIT 1000
//Define my six stepper motors
//NOTE: REQUIRES ARDUINO MEGA
/*
Stepper stepperOne(STEPS,8,9,10,11);
Stepper stepperTwo(STEPS,22,23,24,25);
Stepper stepperThree(STEPS,26,27,28,29);
Stepper stepperFour(STEPS,30,31,32,33);
Stepper stepperFive(STEPS,34,35,36,37);
Stepper stepperSix(STEPS,38,39,40,41);
*/
Stepper stepper(STEPS,8,9,10,11);

//Hard stop sensors for the motors, simple metal contact in this case
//as I don't have to worry about wire count and this is more cost effective
int hsPin = 44;
int homePin = 42;
int ledWarning = 43;
int hardstopRead = 0;
int homeRead = 0;
int STEPLIMIT = 1000;

//This collects the number of steps it takes for a given motor to move to
//a hard stop.
int stepsToHS = 0;

boolean bFirstLoop = true;
boolean bPositiveDirection = true;

void setup(){
  pinMode(ledWarning, OUTPUT);
  pinMode(hsPin, INPUT_PULLUP);
  pinMode(homePin, INPUT_PULLUP);
  stepper.setSpeed(SPEED);  
  Serial.begin(9600);
}

void loop(){
  /*The "first loop" serves two functions. First, it checks that all steppers are at their resting position.
  *later in the program I have a function call that should reset all motors to their zero position.
  *However, if a power outage were to occur I'd want to verify that they are all properly, manually, reset.
  *If I wanted to make this more robust, I'd put in 
  */
  if(bFirstLoop){
    if(digitalRead(homePin) == LOW){
      Serial.println("Homing!");
      HomingRoutine();
      Serial.println("Homing complete!");
    }
    else{
      /*
      *Warns the user that the six steppers are not in their
      *resting position. Should only be needed if a sudden power outage
      *occured.
      */
      digitalWrite(ledWarning, HIGH);
      //Serial.println("Not at rest position.");
    }
  }
  else{
    Serial.println("Activating motor!");
    digitalWrite(ledWarning, HIGH);
    ActivateMotor(bPositiveDirection);
    Serial.println("Motion complete.");
    digitalWrite(ledWarning, LOW);
    bPositiveDirection = !bPositiveDirection;
    delay(1000);
  }
}

void HomingRoutine(){  
  digitalWrite(ledWarning, LOW); 
  int iterator = 1;
  hardstopRead = HIGH;
  while(hardstopRead == HIGH && iterator < STEPLIMIT){
    Serial.print("Stepping: ");
    Serial.println(iterator);
    stepper.step(1);
    hardstopRead = digitalRead(hsPin);
    iterator++;
  }
  stepsToHS = (iterator-2);//I put in a gap of 10 steps as a precaution.
  //Resets the motor
  delay(1000);
  Serial.println("Resetting motor to home pos");
  stepper.step(-1*(iterator-1));
  
  bFirstLoop = false;      
  stepper.setSpeed(SPEED);
  delay(100);
}

/*NOTE: The step() function will block all other functions from taking place
*for the purposes of this project I don't need to have my motors running in sync
*so I'm free to just use this. To do something else I'd pretty much need to make
*something custom here, like having incremental steps with all motors that activate
*until they get to their hard stops.
*/
void ActivateMotor(boolean isPositive){
  int steps = 0;
  if(isPositive){
    steps = stepsToHS;
  }
  else{
    steps = -stepsToHS;
  }
  stepper.step(steps);
}
