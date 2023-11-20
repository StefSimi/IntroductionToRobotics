#include <EEPROM.h>

int input;
int menuState = 6;
byte needInput = true;
const int zeroAscii = 48;

char incomingByte = 0;    // for incoming serial data
String inputString = "";  // a String to hold incoming data
String RGBString;

bool inputComplete = false;
bool seeReadings = false;
int automaticMode;
int stopInput;

// Initial configuration constants
const int initialSamplingInterval = 1000;
const int initialUltrasonicThreshold = 2048;
const int initialLDRThreshold = 2048;

// Pin assignments for RGB LED
const int redPin = 3;
const int greenPin = 5;
const int bluePin = 6;
int redValue;
int greenValue;
int blueValue;

// Menu states for easy navigation
#define SENSOR_SETTINGS 1
#define RESET_DATA 2
#define SYSTEM_STATUS 3
#define RGB_CONTROLS 4
#define EXECUTING_CODE 5
#define MAIN_MENU 6

#define SENSOR_SAMPLING 1
#define ULTRASONIC_THRESHOLD 2
#define LDR_THRESHOLD 3
#define SENSOR_BACK 4

#define RESET_YES 1
#define RESET_NO 2

#define SYSTEM_CURRENT_READINGS 1
#define SYSTEM_CURRENT_SETTINGS 2
#define SYSTEM_LOGGED_DATA 3
#define SYSTEM_BACK 4

#define RGB_MANUAL_INPUT 1
#define RGB_AUTOMATIC 2
#define RGB_BACK 3

#define EXECUTE_SENSOR_SAMPLING 11
#define EXECUTE_ULTRASONIC_THRESHOLD 12
#define EXECUTE_LDR_THRESHOLD 13
#define EXECUTE_CURRENT_READINGS 31
#define EXECUTE_MANUAL_COLOR 41

//Constants to eliminate magic numbers
#define SECOND 1000
#define INT_BYTES 2
#define MAX_RGB_VALUE 255
#define MIN_RGB_VALUE 0
#define BASE_16 16
#define CLEAR_DELAY 2
#define TRANSMISSION_DELAY 10
#define SPEED_OF_SOUND 0.034
#define TWO 2
#define RED_HEX_START 0
#define RED_HEX_STOP 2
#define GREEN_HEX_START 2
#define GREEN_HEX_STOP 4
#define BLUE_HEX_START 4
#define BLUE_HEX_STOP 6
#define CHARACTER_0 '0'
#define CHARACTER_9 '9'
#define CHARACTER_UPPERCASE_A 'A'
#define CHARACTER_UPPERCASE_F 'F'
#define CHARACTER_LOWERCASE_A 'a'
#define CHARACTER_LOWERCASE_F 'f'
#define MAX_SAMPLING_INPUT 10
#define MIN_SAMPLING_INPUT 1
#define MAX_THRESHOLD_INPUT 2047
#define MIN_THRESHOLD_INPUT 0

const int photocellPin = A0;
int photocellValue;
const int trigPin = 9;
const int echoPin = 10;
int ultrasonicThreshold;
int LDRThreshold;
int samplingInterval;
int samplingInput;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
int codeToExecute = 0;
byte executingDone = false;
byte invalid = false;

// EEPROM memory addresses
const int ultrasonicThresholdByte = 0;
const int LDRThresholdByte = 2;
const int samplingByte = 4;
const int counterByte = 6;
const int automaticModeByte = 8;
const int ultrasonicStartingByte = 10;
const int LDRStartingByte = 30;
const int maxReadings = 10;
const int redByte = 50;
const int greenByte = 52;
const int blueByte = 54;

int savedReadingsCounter;

long duration = 0;
int distance = 0;
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(photocellPin, INPUT);
  Serial.begin(9600);
  printMainMenu();

  // Retrieve saved settings from EEPROM
  EEPROM.get(ultrasonicThresholdByte, ultrasonicThreshold);
  EEPROM.get(LDRThresholdByte, LDRThreshold);
  EEPROM.get(samplingByte, samplingInterval);
  EEPROM.get(counterByte, savedReadingsCounter);
  EEPROM.get(automaticModeByte, automaticMode);
}

void loop() {

  menu();  // Handle user input and navigate through the menu

  // Sensor reading and RGB LED control logic
  currentMillis = millis();
  if (currentMillis - previousMillis > samplingInterval) {
    // Ultrasonic sensor readings
    digitalWrite(trigPin, LOW);
    delayMicroseconds(CLEAR_DELAY);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(TRANSMISSION_DELAY);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    distance = duration * SPEED_OF_SOUND / TWO;
    previousMillis = currentMillis;
    photocellValue = analogRead(photocellPin);

    // Save sensor readings to EEPROM
    addSensorReading(photocellValue, LDRStartingByte);
    addSensorReading(distance, ultrasonicStartingByte);


    // Update saved readings counter
    if (savedReadingsCounter < maxReadings * INT_BYTES) {
      savedReadingsCounter += INT_BYTES;

      EEPROM.put(counterByte, savedReadingsCounter);
    }
    // Display readings if enabled
    if (seeReadings) {
      Serial.print("Ultrasonic: ");
      Serial.println(distance);
      Serial.print("LDR: ");
      Serial.println(photocellValue);
      Serial.println("Press any key to exit\n");
    }

    // Automatic RGB LED control based on sensor readings
    if (automaticMode) {
      if (distance >= ultrasonicThreshold || photocellValue >= LDRThreshold) {
        redValue = MAX_RGB_VALUE;
        greenValue = MIN_RGB_VALUE;
        blueValue = MIN_RGB_VALUE;
      } else {
        redValue = MIN_RGB_VALUE;
        greenValue = MAX_RGB_VALUE;
        blueValue = MIN_RGB_VALUE;
      }
    } else {
      
      EEPROM.get(redByte, redValue);
      EEPROM.get(greenByte, greenValue);
      EEPROM.get(blueByte, blueValue);
    }
    // Set the RGB LED colors
    analogWrite(redPin, redValue);
    analogWrite(greenPin, greenValue);
    analogWrite(bluePin, blueValue);
  }
}

// Menu navigation logic
void menu() {
  if (Serial.available() > 0) {

    switch (menuState) {
      case MAIN_MENU:
        {
          if (needInput) {
            getInput(input);//Get an input only if the program is not waiting for another specific input (eg. Color Hex)
          }

          if (inputComplete == true) {//In case there are multiple characters in the input, we need to do this additional check
            inputComplete = false;
            switch (input) {
              case SENSOR_SETTINGS:
                {
                  menuState = SENSOR_SETTINGS;
                  printSensorSettingsMenu();
                  break;
                }
              case RESET_DATA:
                {
                  menuState = RESET_DATA;
                  printResetDataMenu();
                  break;
                }
              case SYSTEM_STATUS:
                {
                  printSystemStatusMenu();
                  menuState = SYSTEM_STATUS;
                  break;
                }
              case RGB_CONTROLS:
                {
                  printRGBControlMenu();
                  menuState = RGB_CONTROLS;
                  break;
                }
              default:
                {
                  //printInvalidMessage();
                  invalid = true;
                  break;
                }

                break;
            }
          }
          case SENSOR_SETTINGS:
            {

              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case SENSOR_SAMPLING:  //1.1 
                    {
                      Serial.println("Enter sensor sampling value (between 1 and 10):");
                      needInput = false;
                      menuState = EXECUTING_CODE;
                      codeToExecute = EXECUTE_SENSOR_SAMPLING;
                      break;
                    }
                  case ULTRASONIC_THRESHOLD:  //1.2 
                    {
                      Serial.print("Current Ultrasonic alert threshold is ");
                      Serial.println(ultrasonicThreshold);
                      Serial.println("Enter new Ultrasonic alert threshold minimum value:");
                      needInput = false;
                      menuState = EXECUTING_CODE;
                      codeToExecute = EXECUTE_ULTRASONIC_THRESHOLD;

                      break;
                    }
                  case LDR_THRESHOLD:  //1.3 
                    {
                      Serial.print("Current LDR alert threshold is ");
                      Serial.println(LDRThreshold);
                      Serial.println("Enter new LDR alert threshold minimum value:");
                      needInput = false;

                      //ULTRASONIC_THRESHOLD code;
                      menuState = EXECUTING_CODE;
                      codeToExecute = EXECUTE_LDR_THRESHOLD;

                      break;
                    }
                  case SENSOR_BACK:  //1.4 
                    {
                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      //printInvalidMessage();
                      invalid = true;
                      break;
                    }
                }
              }

              break;
            }
          case RESET_DATA:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                //inputString="";
                inputComplete = false;
                switch (input) {
                  case RESET_YES:  //2.1 DONE
                    {
                      Serial.println("Resetting Data...");
                      //RESET DATA CODE
                      EEPROM.put(ultrasonicThresholdByte, initialUltrasonicThreshold);
                      EEPROM.put(LDRThresholdByte, initialLDRThreshold);
                      EEPROM.put(samplingByte, initialSamplingInterval);
                      EEPROM.get(ultrasonicThresholdByte, ultrasonicThreshold);
                      EEPROM.get(LDRThresholdByte, LDRThreshold);
                      EEPROM.get(samplingByte, samplingInterval);
                      resetReadings();
                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  case RESET_NO:  //2.2 DONE
                    {
                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }
              break;
            }
          case SYSTEM_STATUS:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case SYSTEM_CURRENT_READINGS:  //3.1 
                    {
                      Serial.println("Printing current readings. Press any key to exit...");
                      //SYSTEM_CURRENT_READINGS CODE
                      seeReadings = true;
                      menuState = EXECUTING_CODE;
                      codeToExecute = EXECUTE_CURRENT_READINGS;
                      //printMainMenu();
                      break;
                    }
                  case SYSTEM_CURRENT_SETTINGS:  //3.2 
                    {
                      Serial.println("Current Settings:");
                      Serial.print("Current Sampling interval: ");
                      Serial.println(samplingInterval);
                      Serial.print("Current Ultrasonic alert threshold: ");
                      Serial.println(ultrasonicThreshold);
                      Serial.print("Current LDR alert threshold: ");
                      Serial.println(LDRThreshold);
                      Serial.println("");
                      //SYSTEM_CURRENT_SETTINGS CODE
                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  case SYSTEM_LOGGED_DATA:  //3.3 
                    {
                      Serial.println("Ultrasonic data:");
                      for (int i = ultrasonicStartingByte; i < savedReadingsCounter + ultrasonicStartingByte - INT_BYTES; i += INT_BYTES) {
                        int x;
                        EEPROM.get(i, x);
                        Serial.print(x);
                        Serial.print(" ");
                      }
                      Serial.println("\n\nLDR data");
                      for (int i = LDRStartingByte; i < savedReadingsCounter + LDRStartingByte - INT_BYTES; i += INT_BYTES) {
                        int x;
                        EEPROM.get(i, x);
                        Serial.print(x);
                        Serial.print(" ");
                      }
                      Serial.println("");


                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  case SYSTEM_BACK:  //3.4 
                    {
                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }
              break;
            }
          case RGB_CONTROLS:
            {
              if (needInput) {
                getInput(input);
              }
              if (inputComplete == true) {
                inputComplete = false;
                switch (input) {
                  case RGB_MANUAL_INPUT:  //4.1 
                    {
                      Serial.println("Enter an RGB value in HEX format:");
                      needInput = false;
                      menuState = EXECUTING_CODE;
                      codeToExecute = EXECUTE_MANUAL_COLOR;
                      //printMainMenu();
                      break;
                    }
                  case RGB_AUTOMATIC:  //4.2 
                    {
                      Serial.println("RBG Automatic Mode has been toggled...");
                      //RGB_AUTOMATIC CODE
                      menuState = MAIN_MENU;
                      automaticMode = !automaticMode;
                      EEPROM.put(automaticModeByte, automaticMode);
                      printMainMenu();
                      break;
                    }
                  case RGB_BACK:  //4.3 
                    {
                      menuState = MAIN_MENU;
                      printMainMenu();
                      break;
                    }
                  default:
                    {
                      invalid = true;
                      break;
                    }
                }
              }
              break;
            }
          case EXECUTING_CODE:
            {
              //code for functions here
              switch (codeToExecute) {
                case EXECUTE_SENSOR_SAMPLING:
                  {
                    sensorSampling();
                    break;
                  }
                case EXECUTE_ULTRASONIC_THRESHOLD:
                  {
                    alertThreshold(ultrasonicThreshold, ultrasonicThresholdByte);
                    break;
                  }
                case EXECUTE_LDR_THRESHOLD:
                  {
                    alertThreshold(LDRThreshold, LDRThresholdByte);
                    break;
                  }
                case EXECUTE_CURRENT_READINGS:
                  {
                    currentSensorReadings();
                    break;
                  }
                case EXECUTE_MANUAL_COLOR:
                  {
                    changeColor();
                    break;
                  }
              }

              if (executingDone) {
                executingDone = false;
                printMainMenu();
                menuState = MAIN_MENU;
                needInput = true;
              }
              break;
            }
          default:
            {
              invalid = true;
              //printInvalidMessage();
              break;
            }
        }
    }
    if (invalid) {
      printInvalidMessage();
      invalid = false;
    }
  }
}

void getInput(int &input) {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = (char)Serial.read();
    inputString += incomingByte;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (incomingByte == '\n') {
      input = atoi(inputString.c_str());
      inputString = "";
      inputComplete = true;
    }
  }
}

void getRGBInput(String &RGBString) {
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = (char)Serial.read();
    inputString += incomingByte;
    if (incomingByte == '\n') {
      RGBString = inputString;

      inputString = "";
      inputComplete = true;
    }
  }
}



void printMainMenu() {
  Serial.println("Main Menu");
  Serial.println("1. Sensor Settings");
  Serial.println("2. Reset Logger Data");
  Serial.println("3. System Status");
  Serial.println("4. RGB LED Control\n");
}

void printSensorSettingsMenu() {
  Serial.println("1. Sensor Settings");
  Serial.println("    1. Sensor Sampling Interval");
  Serial.println("    2. Ultrasonic Alert Threshold");
  Serial.println("    3. LDR Alert Threshold");
  Serial.println("    4. Back\n");
}

void printResetDataMenu() {
  Serial.println("2. Reset Logger Data");
  Serial.println("    Are you sure?");
  Serial.println("    1. Yes");
  Serial.println("    2. No\n");
}

void printSystemStatusMenu() {
  Serial.println("3. System Status");
  Serial.println("    1. Current Sensor Readings");
  Serial.println("    2. Current Sensor Settings");
  Serial.println("    3. Display Logged Data");
  Serial.println("    4. Back\n");
}

void printRGBControlMenu() {
  Serial.println("4. RGB LED Control");
  Serial.println("    1. Manual Color Control");
  Serial.println("    2. LED: Toggle Automatic ON/ OFF");
  Serial.println("    3. Back\n");
}

void printInvalidMessage() {
    Serial.println("Invalid Input");
}


void sensorSampling() {
  getInput(samplingInput);
  if (inputComplete) {
    inputComplete = false;
    //inputString="";
    if (samplingInput > MAX_SAMPLING_INPUT || samplingInput < MIN_SAMPLING_INPUT) {
      printInvalidMessage();
    } else {
      samplingInterval = samplingInput * SECOND;
      EEPROM.put(samplingByte, samplingInterval);
      executingDone = true;
    }
  }
}

void alertThreshold(int &threshold, const int byte) {
  getInput(threshold);
  if (inputComplete) {
    inputComplete = false;
    if (threshold < MIN_THRESHOLD_INPUT || threshold > MAX_THRESHOLD_INPUT) {
      printInvalidMessage();
    } else {
      EEPROM.put(byte, threshold);
      //return threshold;
      Serial.println(threshold);
      executingDone = true;
    }
  }
}

void addSensorReading(int reading, int startingByte) { // function that shifts all previous readings to the right by 2 bytes, because we store data as integers, and then adds the new value to the EEPROM

  for (int i = savedReadingsCounter - INT_BYTES + startingByte; i > startingByte; i -= INT_BYTES) {
    int currentValue;
    EEPROM.get(i - INT_BYTES, currentValue);
    EEPROM.put(i, currentValue);
  }
  EEPROM.put(startingByte, reading);

}
void resetReadings() {
  for (int i = 0; i < maxReadings * INT_BYTES; i += INT_BYTES) {
    EEPROM.put(ultrasonicStartingByte + i, 0);
    EEPROM.put(LDRStartingByte + i, 0);
  }
  EEPROM.put(counterByte, 0);
  EEPROM.put(redByte, 0);
  EEPROM.put(greenByte, 0);
  EEPROM.put(blueByte, 0);
  EEPROM.put(automaticModeByte, 0);

  EEPROM.get(counterByte, savedReadingsCounter);
  EEPROM.get(redByte, redValue);
  EEPROM.get(greenByte, greenValue);
  EEPROM.get(blueByte, blueValue);
  EEPROM.get(automaticModeByte, automaticMode);
}

void currentSensorReadings() {
  getInput(stopInput);
  if (inputComplete) {
    inputComplete = false;
    seeReadings = false;
    executingDone = true;
  }
}


void changeColor() {// function that handles manual input for a color in HEX format
  getRGBInput(RGBString);
  if (inputComplete) {
    inputComplete = false;
    if (isValidHexColor(RGBString)) {
      // Convert the string to RGB values
      redValue = hexToDec(RGBString.substring(RED_HEX_START, RED_HEX_STOP));
      greenValue = hexToDec(RGBString.substring(GREEN_HEX_START, GREEN_HEX_STOP));
      blueValue = hexToDec(RGBString.substring(BLUE_HEX_START, BLUE_HEX_STOP));

      EEPROM.put(redByte, redValue);
      EEPROM.put(greenByte, greenValue);
      EEPROM.put(blueByte, blueValue);



      analogWrite(redPin, redValue);
      analogWrite(greenPin, greenValue);
      analogWrite(bluePin, blueValue);
      executingDone = true;



    } else {
      // Handle incorrect format
      Serial.println("Invalid color format");
    }
  }
}


bool isValidHexColor(String color) {

  // Check if the string is a valid hexadecimal color representation
  if (color.length() == BLUE_HEX_STOP+1) {
    for (int i = 0; i < BLUE_HEX_STOP; i++) {
      char c = color.charAt(i);
      if (!isHexadecimalDigit(c)) {
        return false;
      }
    }
    return true;
  }
  return false;
}

int hexToDec(String hexString) {
  // Convert a hexadecimal string to an integer
  return strtol(hexString.c_str(), NULL, BASE_16);
}


bool isHexadecimalDigit(char c) {
  // Check if a character is a valid hexadecimal digit (0-9, A-F, a-f)
  return (c >= CHARACTER_0 && c <= CHARACTER_9) || (c >= CHARACTER_UPPERCASE_A && c <= CHARACTER_UPPERCASE_F) || (c >= CHARACTER_LOWERCASE_A && c <= CHARACTER_LOWERCASE_F);
}
