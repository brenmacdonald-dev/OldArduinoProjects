#define INPUT_SIZE 30
char input[INPUT_SIZE + 1];
int index = 0;
int randIndex = 0;
String inputString = "";
String outString = "";
boolean stringComplete = false;
boolean demoMode = true;
boolean startRead = true;
//unsigned long timestamp[64];
//int motorLog[64];
//int highlow[64];
int TimeLogIndex = 0;

long helper1 = 0;
long helper2 = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  inputString.reserve(200);
  for(int i = 2; i < 9; i++){
    pinMode(i, OUTPUT);
  }
  /*
  for(int i = 0; i < 64; i++){
    timestamp[i] = 0;
    highlow[i] = 0;
    motorLog[i] = 0;
  }
  */
  TimeLogIndex = 0;
  helper1 = micros();
  helper2 = micros();
  //Serial.println(helper1);
  //Serial.println(helper2);
  //Serial.println(micros());
}

void loop() {
  // put your main code here, to run repeatedly:
  if(stringComplete){
    //Serial.println(inputString);    
    char* separator = strchr(input, ',');
    if(separator != 0){
      *separator = 0;
      int solenoidID = atoi(input);
      ++separator;
      int activation = atoi(separator);
      boolean isActive = activation == 1 ? true : false;
      if(solenoidID == -1){
        demoMode = isActive;
      }
      else if(solenoidID == 8){
        digitalWrite(8, isActive ? HIGH : LOW);
      }
      else if(solenoidID == -2){
        /*
        Serial.println(TimeLogIndex);
        delay(10);
        for(int i = 0; i < TimeLogIndex; i++){
          Serial.print(timestamp[i]);
          Serial.print(",");
          Serial.print(motorLog[i]);
          Serial.print(",");
          Serial.println(highlow[i]);
          delay(50);
        }
        for(int i = 0; i < 64; i++){
          timestamp[i] = 0;
          highlow[i] = 0;
          motorLog[i] = 0;
        }
        TimeLogIndex = 0;
        */
      }
      else{
        if(solenoidID >= 1 && solenoidID < 7){
          demoMode = false;
          int pinShift = solenoidID + 1;
          digitalWrite(pinShift, isActive ? HIGH : LOW);
          //timestamp[TimeLogIndex] = micros();
          //highlow[TimeLogIndex] = activation;
          //motorLog[TimeLogIndex] = solenoidID;
          TimeLogIndex++;          
        }        
      }
      //outString = solenoidID + " " + activation;
      //Serial.println(outString);
      Serial.print(solenoidID);
      Serial.print(",");
      Serial.println(activation);
      //outString = "";
      //Serial.print("Solenoid ID: ");
      //Serial.print(solenoidID);
      //Serial.print(" Activation: ");
      //Serial.println(activation);
    }
    index = 0;
    inputString = "";    
    stringComplete = false;
  }
  if(demoMode){
    for(int i=2; i<8; i++){
      digitalWrite(i,HIGH);
      delay(1000);
      digitalWrite(i,LOW);
      delay(1000);
    }    
    delay(1000);
    for(int i=7; i>0; i--){
      digitalWrite(i,HIGH);
      delay(1000);
      digitalWrite(i,LOW);
      delay(1000);
    }
    delay(1000);
    for(int j=0; j<10; j++){
      for(int i=2; i<5; i++){
        digitalWrite(i,HIGH);
        digitalWrite(i+3,HIGH);
        delay(500);
        digitalWrite(i,LOW);
        digitalWrite(i+3,LOW);
        delay(500);
      }
    }
    delay(5000);
    /*
    randIndex = random(2,8);
    digitalWrite(randIndex, HIGH);
    delay(500);
    digitalWrite(randIndex, LOW);
    
    Serial.print("Demo: ");
    Serial.print(randIndex);
    Serial.println(" HIGH");
    delay(300);
    digitalWrite(randIndex, LOW);
    Serial.print("Demo: ");
    Serial.print(randIndex);
    Serial.println(" LOW");
    */
    delay(1000);
  }
}

void serialEvent(){  
  while(Serial.available()){
    char inChar = (char)Serial.read();
    input[index] = inChar;
    index++;
    inputString += inChar;
    if(inChar == '\n'){
      stringComplete = true;
      //Serial.println("Message received!");
    }
  }
}

