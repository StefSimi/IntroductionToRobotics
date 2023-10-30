//If you want to add more buttons change these 3 lines of code, everything should work the same
const int numberOfInputs=5;
const int inputPins[numberOfInputs]={2,3,4,5,6};
const int ledPins[numberOfInputs]={10,11,12,8,13};

const int elevatorLedPin=7;
const int buzzerPin=9;

//Debounce logic
const int debounceDelay=100;
unsigned long lastDebounceTime[numberOfInputs]={0};
byte buttonState[numberOfInputs]={0};
byte lastButtonState[numberOfInputs]={0};
byte reading[numberOfInputs]={0};
byte lastReading[numberOfInputs]={0};
byte inputRequests[numberOfInputs]={0};

//Elevator logic
#define IDLE 0
#define DOORS_CLOSING 1
#define MOVING 2
#define ARRIVING 3
int currentFloor=0;
int targetFloor=-1; //-1 indicates there is no current floor to go to
int elevatorState=IDLE;

const int doorCloseTime=1000;
const int blinkToFloorChangeRatio=8;
unsigned long doorCloseReading;
int floorDifference;
int direction;
unsigned long moveReading;
unsigned long moveLastReading;
int moveCounter=0;
byte movingLedState=HIGH;
int movingLedSwapInterval=500;

//Music logic
int musicCounter;
unsigned long musicTimer;
unsigned long ringBellTimer;

#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_DS5 622
#define NOTE_F5 698
#define NOTE_G5 784



void setup() {
  pinMode(elevatorLedPin,OUTPUT);
  pinMode(buzzerPin,OUTPUT);
  for(int i=0;i<numberOfInputs;i++){
    pinMode(inputPins[i],INPUT_PULLUP);
    pinMode(ledPins[i],OUTPUT);
  }
  Serial.begin(9600);
  
}

void loop() {
  
  for(int i=0;i<numberOfInputs;i++){
    debounce(i);
  }
 

  
  switch (elevatorState){

    case IDLE:{//The elevator is stationary and is waiting for inputs
        digitalWrite(ledPins[currentFloor],HIGH);
        digitalWrite(elevatorLedPin,HIGH);
        int i=currentFloor-1;
        int j=currentFloor+1;
        while(i>=0&&j<numberOfInputs){//We go in both ways at the same time so we can make sure that we go to closest floor to the current elevator position
          if(inputRequests[i]&&i!=currentFloor){
            startClosingDoors(i);
            break;
          }

          if(inputRequests[j]&&i!=currentFloor){
            startClosingDoors(j);
            break;
          }
          i--;
          j++;
        }

        while(i>=0){
          if(inputRequests[i]&&i!=currentFloor){
            startClosingDoors(i);
            break;
          }
          i--;
        }

        while(j<numberOfInputs){//Even though there are 3 while loops, there will be at most numberOfInputs operations being made 
          if(inputRequests[j]&&i!=currentFloor){
            startClosingDoors(j);
            break;
          }
          j++;
        }

        break;
      }

    case DOORS_CLOSING:{//Elevator doors are closing, check if there is a closer floor than the target floor in the same direction that it can first go to
        digitalWrite(ledPins[currentFloor],HIGH);
        digitalWrite(elevatorLedPin,HIGH);

        closeDoorSound(musicCounter);//We play a door closing sound 
        
        if(floorDifference>0){
          direction=1; //Knowing if we are going either up or down helps trivialize going up or down multiple floors
          for(int i=currentFloor+1;i<targetFloor;i++){
            if(inputRequests[i]){
              targetFloor=i;
              break;
            } 
          }
        }
        else{
          direction=-1;
          for(int i=currentFloor-1;i>targetFloor;i--){
            if(inputRequests[i]){
              targetFloor=i;
              break;
            } 
          }
        }
        if((millis()-doorCloseReading)>doorCloseTime){
          
          elevatorState=MOVING;
          musicTimer=millis();
          moveCounter=0;
          musicCounter=0;
          
        }
        break;
      }

    case MOVING:{ //Elevator is going to targetFloor
      moveReading=millis();

      elevatorMusicSound(musicCounter); //When the elevator is moving we play some quality elevator music :)
      if(moveReading-moveLastReading>=movingLedSwapInterval){
        moveLastReading=moveReading;
        movingLedState=!movingLedState;
        moveCounter++;
        
        if(moveCounter%blinkToFloorChangeRatio==0){ //We calculate how many times the elevator led is changing states (blinking), and if it does so for a set number of times, we can go to the next floor
          currentFloor+=direction;
        }
        if(currentFloor==targetFloor){
          inputRequests[targetFloor]=0;
          targetFloor=-1;

          ringBellTimer=millis();
          musicCounter=0;
          noTone(buzzerPin);
          elevatorState=ARRIVING;
        }

      }
      if((moveCounter/(blinkToFloorChangeRatio/2))%2==0){ //When the elevator is between floors no floor leds should be on
        digitalWrite(ledPins[currentFloor],HIGH);
      }
      else{
        digitalWrite(ledPins[currentFloor],LOW);
      }
      
      digitalWrite(elevatorLedPin,movingLedState);
      break;
      
      }

    case ARRIVING:{//The elevator has arrived at its destination, and we will play a happy jingle sound

      ringBellSound(musicCounter);
        
      }

    default:
      break;
  }
  
}

void debounce(int button){//Insead of copy pasing the debounce code for each button, we create a funciton that will debounce a specified button, so we can have any number of buttons
  reading[button] = digitalRead(inputPins[button]);
  if (reading[button] != lastReading[button]) {
    lastDebounceTime[button] = millis();
  }

  if ((millis() - lastDebounceTime[button]) > debounceDelay) {
    if (reading[button] != buttonState[button]) {
      buttonState[button] = reading[button];
      if (buttonState[button] == LOW && currentFloor != button) {
        inputRequests[button]=1;
      }
    }
  }
  lastReading[button] = reading[button];
}

void startClosingDoors(int target){//We need to change some variables when we decide to go to a different floor, utility function
  targetFloor=target;
  elevatorState=DOORS_CLOSING;
  doorCloseReading=millis();
  musicCounter=0;
  floorDifference = targetFloor-currentFloor;
}

void ringBellSound(int &musicCounter){
  switch (musicCounter){
    case 0:{
      if(millis()-ringBellTimer>100)
      musicCounter++;
      tone(buzzerPin,NOTE_C5,1000);
      break;
    }
    case 1:{
      if((millis()-ringBellTimer)>200){
        musicCounter++;
        tone(buzzerPin,NOTE_DS5,500);
      }
      break;
    }
    case 2:{
      if((millis()-ringBellTimer)>300){
        musicCounter++;
        tone(buzzerPin,NOTE_F5,1000);
      }
      break;
    }
    case 3:{
      if((millis()-ringBellTimer)>400){
        musicCounter++;
        tone(buzzerPin,NOTE_G5,1500);
      }
      break;
    }
    case 4:{
      if((millis()-ringBellTimer)>2250){
        elevatorState=IDLE;
      }
      break;
    }
  }
  
}

void closeDoorSound(int &musicCounter){
  switch (musicCounter){
    case 0:{
      musicCounter++;
      tone(buzzerPin,NOTE_C5,400);
      break;
    }
    case 1:{
      if((millis()-doorCloseReading)>100){
        musicCounter++;
        tone(buzzerPin,NOTE_A4,800);
      }
      break;
    }
    case 2:{
      if((millis()-doorCloseReading)>300){
        musicCounter++;
        tone(buzzerPin,NOTE_F4,400);
      }
    }
    default:{
      break;
    }
  }
}

void elevatorMusicSound(int &musicCounter){
  switch (musicCounter){
    case 0:{
      musicCounter++;
      tone(buzzerPin,NOTE_C5,400);
      break;
    }
    case 1:{
      if((millis()-musicTimer)>200){
        musicCounter++;
        tone(buzzerPin,NOTE_A4,400);
      }
      break;
    }
    case 2:{
      if((millis()-musicTimer)>400){
        musicCounter++;
        tone(buzzerPin,NOTE_B4,400);
      }
      break;
    }
    case 3:{
      if((millis()-musicTimer)>600){
        musicCounter++;
        tone(buzzerPin,NOTE_C5,400);
      }
      break;
    }
    case 4:{
      if((millis()-musicTimer)>1000){
        musicCounter=0;
        musicTimer=millis();
      }
      break;
    }
  }
}