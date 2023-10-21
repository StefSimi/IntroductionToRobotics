const int redInputPin=A0;
const int greenInputPin=A1;
const int blueInputPin=A2;
//Input method could be changed in the future, so the variables shouldn't have the name of a specific electronic component (eg. potentiometer/ slider)

const int redLedPin=9;
const int greenLedPin=10;
const int blueLedPin=11;
//Pins 9, 10 and 11 are PWM pins

const int minLedIntensity=0
const int maxLedIntensity=255;
const int minInputValue=0;
const int maxInputValue=1023;

//We should be able to change these values as needed instead of using magic numbers in the map() funciton. For example, we may want to decrease the maximum led intensity,
//because looking at a led that's glowing at full power can be very tiring for your eyes.

int redInputValue=0;
int greenInputValue=0;
int blueInputValue=0;
//This is where we are going to store our input values

int redLedValue=0;
int greenLedValue=0;
int blueLedValue=0;
//This is where we are goint to store our calculated output values

void setup() {
  pinMode(redInputPin, INPUT);
  pinMode(greenInputPin, INPUT);
  pinMode(blueInputPin, INPUT);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

}
 
void loop() {
  redInputValue=analogRead(redInputPin);
  greenInputValue=analogRead(greenInputPin);
  blueInputValue=analogRead(blueInputPin);

  redLedValue=map(redInputValue, minInputValue, maxInputValue, minLedIntensity, maxLedIntensity);
  greenLedValue=map(greenInputValue, minInputValue, maxInputValue, minLedIntensity, maxLedIntensity);
  blueLedValue=map(blueInputValue, minInputValue, maxInputValue, minLedIntensity, maxLedIntensity);
  //Using the map() funciton gives more precise results than just simply dividing the input value by 4 

  analogWrite(redLedPin,redLedValue);
  analogWrite(greenLedPin,greenLedValue);
  analogWrite(blueLedPin,blueLedValue);
}