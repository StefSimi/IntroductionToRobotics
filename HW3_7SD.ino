//Display pins
const int pinA = 12;
const int pinB = 10;
const int pinC = 9;
const int pinD = 8;
const int pinE = 7;
const int pinF = 6;
const int pinG = 5;
const int pinDP = 4;
const int segSize = 8;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

//Joystick pins + logic
const int pinSW = 2;  // digital pin connected to switch output
const int pinX = A0;  // A0 - analog pin connected to X output
const int pinY = A1;  // A1 - analog pin connected to Y output
int xValue = 0;
int yValue = 0;
const int joystickDeadZone = 60; //the zone in the center of the joystick
const int joystickBuffer = 75; //a buffer so the joystick doesn't continuously read 2 values back and forward
const int joystickCenter = 512;
const int joystickRange = 1023;


//Debounce logic
volatile bool buttonPressed = false;
volatile unsigned long lastInterruptTime = 0;

const unsigned long debounceDelay = 50;
const unsigned long minButtonLongPressDuration = 2000;
byte swState;
unsigned long buttonLongPressMicros;
bool buttonStateLongPress = false;
unsigned long buttonPressDuration;
unsigned long currentMicros;
unsigned long previousMillis;
int buttonStatePrevious = LOW;


//Blink logic
const int blinkInterval = 500;
unsigned long currentMillis;
unsigned long previousBlinkMillis;
byte currentLedState = LOW;
byte commonAnode = true;
byte initialState;

#define UP_STATE 0
#define DOWN_STATE 1
#define LEFT_STATE 2
#define RIGHT_STATE 3
#define IDLE_STATE 4

int joystickState = IDLE_STATE;

int segmentState[segSize] = { 0 };

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define DP 7

int travelMatrix[segSize][4] = {  //4 directions
 // U  D  L  R
  { A, G, F, B },
  { A, G, F, B },
  { G, D, E, DP },
  { G, D, E, C },
  { G, D, E, C },
  { A, G, F, B },
  { A, D, G, G },
  { DP, DP, C, DP }
};


int currentSegment = 7;


void setup() {
  initialState = commonAnode;
  // Initialize all the pins
  pinMode(pinSW, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(pinSW), handleInterrupt, RISING);
  Serial.begin(9600);
  reset();
}

void loop() {
  if (buttonPressed) {//Debouncing with interrupts is very tricky to implement if you want to decect long presses 
    //segmentState[currentSegment] = !segmentState[currentSegment];
    buttonPressed = false;
  }
  currentMillis = millis();
  if (currentMillis - previousMillis > debounceDelay) {
    swState = !digitalRead(pinSW);
    if (swState == HIGH && buttonStatePrevious == LOW && !buttonStateLongPress) {//The button has been pressed, we start a timer to check for long the button has been pressed if one has not started already
      buttonLongPressMicros = currentMillis;
      buttonStatePrevious = HIGH;
    }
    buttonPressDuration = currentMillis - buttonLongPressMicros;
    if (swState == HIGH && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {//The button is still pressed, but we detect a long press, so we reset the current position and turn all LEDs off.
      buttonStateLongPress = true;
      reset();
    }

    if (swState == LOW && buttonStatePrevious == HIGH) {//The button has been released
      buttonStatePrevious = LOW;
      buttonStateLongPress = false;
      if (!buttonStateLongPress && buttonPressDuration < minButtonLongPressDuration) {//If it has been pressed for a shorter time than minButtonLongPressDuration, then we toggle the current positon LED.
        segmentState[currentSegment] = !segmentState[currentSegment];
      }
    }
    previousMillis = currentMillis;
  }
  handleMovement();

  currentMillis = millis();
  if (currentMillis - previousBlinkMillis >= blinkInterval) {//We blink the current LED
    previousBlinkMillis = currentMillis;
    currentLedState = !currentLedState;
    digitalWrite(segments[currentSegment], currentLedState);
  }
}

void handleMovement() {
  //We can consider our input range to be a 1024 by 1024 matrix
  //If we are in the center of the matrix, then we should set our status to Idle. However, the joystick is not perfect, so it will not read x=512 and y=512 when it is let go.
  //Therefore, we create a dead zone around the center of the joystick

  //In order to detect the side we are on (up,down,left or right), we can make use of our matrix once again.
  //We know that when the joystick is all the way down to the bottom left positon, it will read x=0, y=0, and x=1023, y=1023 if it is in the top right position
  //Using basic algebra, we can deduce that if x > y, then the joystick is in the top left of the matrix, and if x < y, then the joystick in in the bottom right of the matrix
  //We can also deduce that if x+y is greater than 1024 (the maximum value), the joystick is in the top right of the matrix, otherwise it is in the bottom left
  //So, by combining these 2 conditions, we can know for sure if the joystick is located up, down, left or right
  //There is still a problem however: if the joystick is very close to 2 sides (eg. top right), then it may start to continuously toggle between the 2 states, which can cause problems.
  //To fix this we create a buffer zone, where it will be considered idle.
  //Technically, you can go from the center of the joystick to a corner without it reading anything, but realistically this is almost impossible to do

  //To visualise things better, below is a 7x7 matrix represented in the same way that our joystick matrix is:
  //60  61  62  63  64  65  66
  //50	51	52	53	54	55	56
  //40	41	42	43	44	45	46
  //30	31	32	33	34	35	36
  //20	21	22	23	24	25	26
  //10	11	12	13	14	15	16
  //00	11	22	33	44	55	66

  int xValue = analogRead(A0);  // Read X-axis value
  int yValue = analogRead(A1);  // Read Y-axis value
  int coordSum = xValue + yValue;
  if ((xValue > joystickCenter - joystickDeadZone && xValue < joystickCenter + joystickDeadZone && yValue > joystickCenter - joystickDeadZone && yValue < joystickCenter + joystickDeadZone)//check if we are in the dead zone 
      || abs(xValue - yValue) < joystickBuffer //buffer for secondary diagonal
      || abs(xValue - yValue) > joystickRange - joystickBuffer) {//buffer for main diagonal
    joystickState = IDLE_STATE;
  } else if (xValue >= yValue && coordSum > joystickRange) {//if x > y and their sum is greater than 1023, then the joystick is in the UP position
    //UP
    if (joystickState != UP_STATE) {
      Serial.println("UP");
      joystickState = UP_STATE;
      if (currentSegment != travelMatrix[currentSegment][UP_STATE]) {
        //before moving to another segment using our travel matrix, we need to set our current Segment to its corresponding state, so it doesn't remain in its current blinking position
        digitalWrite(segments[currentSegment], segmentState[currentSegment]); 
        currentSegment = travelMatrix[currentSegment][UP_STATE];
      }
    }
  } else if (xValue > yValue && coordSum < joystickRange) {//if x > y and their sum is not greater than 1023, then the joystick is in the LEFT position
    //LEFT
    if (joystickState != LEFT_STATE) {
      Serial.println("LEFT");
      joystickState = LEFT_STATE;
      if (currentSegment != travelMatrix[currentSegment][LEFT_STATE]) {
        digitalWrite(segments[currentSegment], segmentState[currentSegment]);
        currentSegment = travelMatrix[currentSegment][LEFT_STATE];
      }
    }
  } else if (xValue < yValue && coordSum > joystickRange) {//if x < y and their sum is  greater than 1023, then the joystick is in the RIGHT position
    //RIGHT
    if (joystickState != RIGHT_STATE) {
      Serial.println("RIGHT");
      joystickState = RIGHT_STATE;
      if (currentSegment != travelMatrix[currentSegment][RIGHT_STATE]) {
        digitalWrite(segments[currentSegment], segmentState[currentSegment]);
        currentSegment = travelMatrix[currentSegment][RIGHT_STATE];
      }
    }
  } else if (xValue < yValue && coordSum < joystickRange) {//finally, if x < y and their sum is not greater than 1023, then the joystick is in the DOWN position
    //DOWN
    if (joystickState != DOWN_STATE) {
      Serial.println("DOWN");
      joystickState = DOWN_STATE;
      if (currentSegment != travelMatrix[currentSegment][DOWN_STATE]) {
        digitalWrite(segments[currentSegment], segmentState[currentSegment]);
        currentSegment = travelMatrix[currentSegment][DOWN_STATE];
      }
    }
  }
}

void reset() {//If we register a long press, then we set all of our segments to their initial state and move the current segment back to the start
  currentSegment = DP;
  for (int i = 0; i < segSize; i++) {
    digitalWrite(segments[i], initialState);
    segmentState[i] = initialState;
  }
}

void handleInterrupt() {
  static unsigned long interruptTime = 0;
  interruptTime = micros();
  if (interruptTime - lastInterruptTime > debounceDelay * 1000) {
    buttonPressed = true;
  }
  lastInterruptTime = interruptTime;
}
