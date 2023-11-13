#include <IRremote.h>

// Define connections to the shift register
const int latchPin = 11;  // Connects to STCP (latch pin) on the shift register
const int clockPin = 10;  // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;   // Connects to DS (data pin) on the shift register
// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

//Button Pins
const int lapPin = 2;
const int startStopPin = 3;
const int resetPin = 9;



// Store the digits in an array for easy access
int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;  // Number of digits in the display
// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int encodingsNumber = 16;
// Define byte encodings for the hexadecimal characters 0-F
byte byteEncodings[encodingsNumber] = {
  //A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};
// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  // Delay between updates (milliseconds)
unsigned long number = 0;        // The number being displayed
volatile unsigned long lastStartStopInterruptTime = 0;
volatile unsigned long lastLapInterruptTime = 0;
volatile int lastLapTime = 0;
volatile int lastLapStartTime = 0;
const unsigned long interruptDebounceDelay = 250;


const int maxValue = 9999;


#define SECOND 1000

//Debounce Logic
byte reading;
byte lastResetReading;
byte lastLapReading;
unsigned long lastResetDebounceTime;
unsigned long lastLapDebounceTime;
const int debounceDelay = 100;
byte resetButtonState = 0;
byte lapButtonState = 0;
byte resetRequest = 0;
byte lapRequest = 0;

//Lap Logic
const int maxLaps = 4;
int laps[maxLaps] = { 0 };
volatile int savedLapsCounter = 0;
int lapViewCounter = 0;
unsigned long remainingTime = 0;

//Stopwatch State Logic
#define PAUSED_STATE 0
#define RUNNING_STATE 1
#define RESET_STATE 2
#define LAP_VIEW_STATE 3

int stopwatchState = RESET_STATE;
volatile long stopwatchValue = 0;

//Number Writing Logic
#define DP_BYTE B00000001
#define ZERO_BYTE B00000000
#define DECIMAL_DISPLAY 2
#define TEN 10

//Remote Control Logic
#define PLAY_HEX 0xFFC23D
#define RESET_HEX 0xFF906F
#define LAP_HEX 0xFFA857
#define FORWARD_HEX 0xFF02FD
#define BACKWARD_HEX 0xFF22DD
#define ZERO_HEX 0xFF6897
#define ONE_HEX 0xFF30CF
#define TWO_HEX 0xFF18E7
#define THREE_HEX 0xFF7A85
#define FOUR_HEX 0xFF10EF
#define FIVE_HEX 0xFF38C7
#define SIX_HEX 0xFF5AA5
#define SEVEN_HEX 0xFF42BD
#define EIGHT_HEX 0xFF4AB5
#define NINE_HEX 0xFF52AD
#define ONE_HUNDRED_HEX 0xFF9867
#define TWO_HUNDRED_HEX 0xFFB04F

#define ZERO 0
#define ONE 1
#define TWO 2
#define THREE 3
#define FOUR 4
#define FIVE 5
#define SIX 6
#define SEVEN 7
#define EIGHT 8
#define NINE 9
#define ONE_HUNDRED 100
#define TWO_HUNDRED 200

const int receiverPin = 8;
IRrecv receiver(receiverPin);
decode_results results;



void setup() {

  pinMode(startStopPin, INPUT_PULLUP);
  pinMode(resetPin, INPUT_PULLUP);
  pinMode(lapPin, INPUT_PULLUP);

  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  attachInterrupt(digitalPinToInterrupt(startStopPin), handleStartStopInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(lapPin), handleLapInterrupt, FALLING);

  receiver.enableIRIn();

  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}


void loop() {
  //We debounce the reset button and the lap button. If they are pressed then their respective Request variable will turn true
  //We don't need to debounce the Start/ Stop button here, it is being done inside of the interrupt
  debounce(resetPin, lastResetReading, lastResetDebounceTime, resetButtonState, resetRequest);
  debounce(lapPin, lastLapReading, lastLapDebounceTime, lapButtonState, lapRequest);

  //There are quite a lot of things happening with the remote control, so we create a function in order to achieve cleaner code

  handleRemoteInput();

  switch (stopwatchState) {
    case PAUSED_STATE:  //If the Stopwatch is paused, then we simply write its current value
      {
        writeNumber(stopwatchValue);
        if (resetRequest) {
          //RESET STOPWATCH
          stopwatchState = RESET_STATE;
          stopwatchValue = 0;
          resetRequest = 0;
        }
        if (lapRequest) {
          //DO NOTHING
          lapRequest = 0;
        }
        break;
      }
    case RUNNING_STATE:  //If the Stopwatch is running, then we print its current value, and also check if enough time has passed in order to update it
      {
        writeNumber(stopwatchValue);
        if (millis() - lastIncrement > delayCount) {
          if (stopwatchValue < maxValue) {
            stopwatchValue++;
          }
          lastIncrement = millis();
        }
        if (resetRequest) {
          //DO NOTHING
          resetRequest = 0;
        }
        if (lapRequest) {
          //Do nothing here, everything is being handled inside of the interrupt
          lapRequest = 0;
        }
        break;
      }
    case RESET_STATE:  //Visually, nothing changes, except for the stopwatch being reset to 0, but we enter another state in order to enable lap viewing
      {
        writeNumber(stopwatchValue);
        if (resetRequest) {
          //DO NOTHING
          resetRequest = 0;
        }
        if (lapRequest) {
          //We will now start seeing laps
          if (savedLapsCounter > 0) {
            stopwatchState = LAP_VIEW_STATE;
            lapViewCounter = 0;
          }
          lapRequest = 0;
        }
        break;
      }
    case LAP_VIEW_STATE:  //Instead of the timer, we will cycle between the saved laps
      {
        writeNumber(laps[lapViewCounter]);
        if (resetRequest) {
          //By resetting the saved laps counter, we practically delete all of our previous timed laps, because they will be overwritten
          savedLapsCounter = 0;
          resetRequest = 0;
          stopwatchState = RESET_STATE;
        }
        if (lapRequest) {  // This code could've been written inside of the interrupt, but it is written here in order to keep the interrupt as short as possible

          lapViewCounter++;
          if (lapViewCounter == savedLapsCounter) {
            lapViewCounter = 0;
          }
          lapRequest = 0;
        }


        break;
      }
  }
}
void writeReg(int digit) {
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}
void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}
void writeNumber(int number) {

  int currentDigit = displayCount - 1;

  while (number) {  //We write each digit of the number on a different display
    int lastDigit = number % TEN;
    activateDisplay(currentDigit);
    if (currentDigit == DECIMAL_DISPLAY) {  //If our current display is the second to last one, then we should add the decimal point. To do this, we use a bitwise OR operation
      writeReg(byteEncodings[lastDigit] | DP_BYTE);
    } else {
      writeReg(byteEncodings[lastDigit]);
    }
    currentDigit--;
    number /= TEN;
    writeReg(ZERO_BYTE);
  }
  while (currentDigit >= 0) {  //If the number runs out of digits, then we write 0 instead
    int lastDigit = number % TEN;
    activateDisplay(currentDigit);
    if (currentDigit == DECIMAL_DISPLAY) {
      writeReg(byteEncodings[lastDigit] | DP_BYTE);
    } else {
      writeReg(byteEncodings[0]);
    }
    writeReg(ZERO_BYTE);
    currentDigit--;
  }
}


void handleStartStopInterrupt() {
  static unsigned long startStopInterruptTime = 0;
  startStopInterruptTime = micros();
  if (startStopInterruptTime - lastStartStopInterruptTime > interruptDebounceDelay * SECOND) {  //InterruptDebounceDelay is in Millis, we need to multiply it by 1000
    if (stopwatchState == RUNNING_STATE) {
      remainingTime = startStopInterruptTime - lastIncrement * SECOND;  //For precision purposes, we store the remaining time until the stopwatch increments again
      stopwatchState = PAUSED_STATE;
    } else if (stopwatchState == PAUSED_STATE) {
      lastIncrement = (startStopInterruptTime - remainingTime) / SECOND;  //Using the remaining time, we can calculate exactly when the last increment took place
      remainingTime = 0;
      stopwatchState = RUNNING_STATE;
    } else if (stopwatchState == RESET_STATE) {
      lastLapStartTime = stopwatchValue;  //This should normally be 0, but it can be changed using the remote control
      lastIncrement = startStopInterruptTime / SECOND;
      stopwatchState = RUNNING_STATE;
    }
    lastStartStopInterruptTime = startStopInterruptTime;
  }
}

void handleLapInterrupt() {  //Lap time is calculated as the difference between the current stopwatch time and the previous lap time
  static unsigned long lapInterruptTime = 0;
  lapInterruptTime = micros();
  if (lapInterruptTime - lastLapInterruptTime > interruptDebounceDelay * SECOND) {
    if (stopwatchState == RUNNING_STATE) {
      lastLapTime = stopwatchValue - lastLapStartTime;
      lastLapStartTime = stopwatchValue;
      addLapTime(lastLapTime);
    }
    lastLapInterruptTime = lapInterruptTime;
  }
}

void debounce(const int inputPin, byte &lastReading, unsigned long &lastDebounceTime, byte &buttonState, byte &request) {  //Utility function for debouncing a button
  reading = digitalRead(inputPin);
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        request = 1;
      }
    }
  }
  lastReading = reading;
}


void addLapTime(volatile int lapTime) {  //When we add a lap time, we shift the position of all other stored times in the array by 1 to the right
  if (savedLapsCounter < maxLaps) {
    savedLapsCounter++;
  }
  for (int i = savedLapsCounter - 1; i > 0; i--) {
    laps[i] = laps[i - 1];
  }
  laps[0] = lapTime;  //Then we store the most recent lap time
}

void handleRemoteInput() {
  //Remote input works like this:
  //Helpful video showcasing how IR remotes work: https://www.youtube.com/watch?v=9wfWm6RuwPI
  //What we need to know is that the decode function from the IRemote library will store a HEX code in the results.value variable.
  //This HEX code is unique to each individual remote key. So, by pressing different buttons on the remote, we can map out those HEX values.
  //After we know the HEX value for each key, we can assign a functionality inside of the switch case for the keys that we are interested in;
  if (receiver.decode(&results)) {
    switch (results.value) {
      case PLAY_HEX:
        {  //Similar to pressing the Start/Stop Button, but instead of the interrupt being automatically called when the button switches states, we call the function manually
          handleStartStopInterrupt();
          break;
        }
      case LAP_HEX:
        {
          //Same for the Lap button, but we also set the lapRequest variable to 1
          handleLapInterrupt();
          lapRequest = 1;
          break;
        }
      case RESET_HEX:
        {
          //We don't have an interrupt function for the reset button, so we just set the resetRequest variable to 1
          resetRequest = 1;
          break;
        }
      case FORWARD_HEX:
        {
          //Similar to pressing the lap key while in lap viewing mode, it will cycle between laps from left to right
          if (stopwatchState == RESET_STATE) {
            if (savedLapsCounter > 0) {
              stopwatchState = LAP_VIEW_STATE;
              lapViewCounter = 0;
            }
          }
          if (stopwatchState == LAP_VIEW_STATE) {
            lapViewCounter++;
            if (lapViewCounter == savedLapsCounter) {
              lapViewCounter = 0;
            }
          }
          break;
        }
      case BACKWARD_HEX:
        {
          //Cycles between laps from right to left
          if (stopwatchState == RESET_STATE) {
            if (savedLapsCounter > 0) {
              stopwatchState = LAP_VIEW_STATE;
              lapViewCounter = savedLapsCounter - 1;
            }
          }
          if (stopwatchState == LAP_VIEW_STATE) {
            lapViewCounter--;
            if (lapViewCounter < 0) {
              lapViewCounter = savedLapsCounter - 1;
            }
          }
          break;
        }

      //The next buttons will change the starting value of the stopwatch
      case ZERO_HEX:
        {
          introduceNumber(ZERO);
          break;
        }
      case ONE_HEX:
        {
          introduceNumber(ONE);
          break;
        }
      case TWO_HEX:
        {
          introduceNumber(TWO);
          break;
        }
      case THREE_HEX:
        {
          introduceNumber(THREE);
          break;
        }
      case FOUR_HEX:
        {
          introduceNumber(FOUR);
          break;
        }
      case FIVE_HEX:
        {
          introduceNumber(FIVE);
          break;
        }
      case SIX_HEX:
        {
          introduceNumber(SIX);
          break;
        }
      case SEVEN_HEX:
        {
          introduceNumber(SEVEN);
          break;
        }
      case EIGHT_HEX:
        {
          introduceNumber(EIGHT);
          break;
        }
      case NINE_HEX:
        {
          introduceNumber(NINE);
          break;
        }
      case ONE_HUNDRED_HEX:  //Instead of changing the starting value of the stopwatch, we increment it by 100
        {
          if (stopwatchState == RESET_STATE) {
            stopwatchValue += ONE_HUNDRED;
            if (stopwatchValue > maxValue) {
              stopwatchValue = 0;
            }
          }
          break;
        }
      case TWO_HUNDRED_HEX:  //Or 200
        {
          if (stopwatchState == RESET_STATE) {
            stopwatchValue += TWO_HUNDRED;
            if (stopwatchValue > maxValue) {
              stopwatchValue = 0;
            }
          }
          break;
        }

      default:
        {
          Serial.println(results.value, HEX);  //If the current HEX hasn't been treated yet, then it does not currently have a functionality, so we print it out in order to find out its value
          break;
        }
    }
    receiver.resume();
  }
}
void introduceNumber(int number) {  //Utility function for introducing a number
  if (stopwatchState == RESET_STATE) {
    stopwatchValue = stopwatchValue * TEN + number;
    if (stopwatchValue > maxValue) {
      stopwatchValue = 0;
    }
  }
}
