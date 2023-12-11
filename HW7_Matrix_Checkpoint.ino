#include "LedControl.h"  // Include LedControl library for controlling the LED matrix
#include <EEPROM.h>
//Pin declaration
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int playerXPin = A0;
const int playerYPin = A1;
const int bulletXPin = A2;
const int bulletYPin = A3;
//const int buttonPin = 6;
const int leftSwitchPin = 0;
const int rightSwitchPin = 2;
const int buzzerPin = 3;
const int seedPin = 3;  //A pin that is not connected to anything, used as a seed for the random function

const int lcdBrightnessPin = 6;
const int lcdBrightnessModifier = 50;
int lcdBrightness = 3;

const float initialX = 5;  //Initial player position
const float initialY = 5;
const int initialScore = 1000;

//Frame control logic
const int frameDelay = 1000 / 60;  // 60 FPS
unsigned long previousFrameMillis;
unsigned long currentMillis;

//Joystick logic
const float velocity = 0.064;
const int deadZone = 350;
const int bulletDeadZone = 250;
const int middle = 512;


bool running = false;
bool win = false;

bool makeLaserSound = false;
bool makeExplosionSound = false;
bool makeWinSound = false;
bool makeLoseSound = false;
byte laserSoundCounter = 0;
byte explosionSoundCounter = 0;
byte winSoundCounter = 0;
byte loseSoundCounter = 0;
unsigned long soundMillis;

String aboutString[] = { "Blast Radius", "made by Stefan Simion", "Github.com/StefSimi" };
String aboutString2 = "  Blast Radius made by Stefan Simion Github.com/StefSimi  ";
const int aboutMenuSize = 3;
int aboutIndex = 0;
int scrollCounter = 16;

LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // Create an LedControl object to manage the LED matrix
byte matrixBrightness = 2;                                 // Variable to set the brightness level of the matrix

float currentX = 5;
float currentY = 5;

byte hudCounter = 0;
long score = 1000;
long highscore = 0;
#define HIGHSCORE_BYTE 0
#define MATRIX_BRIGHTNESS_BYTE 10
#define LCD_BRIGHTNESS_BYTE 12

#define UP_STATE 0
#define DOWN_STATE 1
#define LEFT_STATE 2
#define RIGHT_STATE 3
#define IDLE_STATE 4

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
#define THIRTY_FRAMES 30

int joystickState = IDLE_STATE;
const int joystickRange = 1024;
const int joystickBuffer = 150;
const unsigned long joystickTimeBuffer = 400;
unsigned long joystickMillis;
const int numberOfOptions = 5;
const int numberOfColumns = 16;

const byte matrixSize = 8;  // Size of the LED matrix

//Blink logic
byte playerFrameCounter = 0;
const byte playerOnInterval = 45;
const byte playerOffInterval = 15;
bool playerBlinkState = 0;
byte bulletFrameCounter = 0;
const byte bulletOnInterval = 10;
const byte bulletOffInterval = 10;
bool bulletBlinkState = 0;

//Bullet logic
const int bulletDelay = 60;
int bulletCounter = 0;
bool bulletFired = 0;
float bulletCurrentX;
float bulletCurrentY;
float bulletXVel;
float bulletYVel;

//Explosion logic
bool explosion = false;
byte explosionCounter = 0;
int explosionX;
int explosionY;

int menuState = 0;

int aboutLineNumber = 1;
int iCursor = 0;

byte reading;
unsigned long debounceDelay = 100;

#define LOSE_IMAGE 0
#define WIN_IMAGE 1
#define SETTINGS_IMAGE 2
#define HOME_IMAGE 3

#define BULLET_VELOCITY_MODIFIER 6000
#define HALF 0.5
#define EXPLOSION_DURATION 60
#define SCORE_PER_KILL 100
#define SCORE_LOSS 250
#define TWO 2



#define FRAME_1 1
#define FRAME_4 4
#define FRAME_7 7
#define FRAME_16 16
#define FREQUENCY_900 900
#define FREQUENCY_1000 1000
#define FREQUENCY_1200 1200
#define FREQUENCY_1500 1500


#define LASER_SOUND_DURATION 50
#define EXPLOSION_SOUND_DURATION_1 100
#define EXPLOSION_SOUND_DURATION_2 200
#define EXPLOSION_SOUND_DURATION_3 150

#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_G4 392
#define NOTE_F4 349
#define NOTE_D4 293
#define NOTE_A3 220

#define WIN_SOUND_DURATION_1 200
#define WIN_SOUND_DURATION_2 800

#define WIN_SOUND_DELAY_1 200
#define WIN_SOUND_DELAY_2 400
#define WIN_SOUND_DELAY_3 600
#define WIN_SOUND_DELAY_4 1400


#define LOSE_SOUND_DURATION_1 300
#define LOSE_SOUND_DURATION_2 600

#define LOSE_SOUND_DELAY_1 150
#define LOSE_SOUND_DELAY_2 300
#define LOSE_SOUND_DELAY_3 450
#define LOSE_SOUND_DELAY_4 750

#define MAIN_MENU 0
#define START_GAME 1
#define SETTINGS 2
#define ABOUT 3
#define EXECUTING_CODE 4
#define ENDGAME 5



const uint64_t IMAGES[] = {
  0x3c66ff9999ffff7e,  //Lose
  0x6090d016690d0106,  //Win
  0x5aff66c3c366ff5a,  //Options
  0xff81bda5a5bd81ff   //Home screen

};
const int IMAGES_LEN = sizeof(IMAGES) / 8;



enum id {
  EMPTY,
  WALL,
  PLAYER,
  ENEMY,
  BULLET
};

byte entityMatrix[matrixSize][matrixSize] = {  //Matrix used internally, for entity control and logic
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, PLAYER, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};
bool matrix[matrixSize][matrixSize] = {  //This is the matrix used for rendering images to the LED matrix
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};
// Array representing each row of the LED matrix as a byte

#include <LiquidCrystal.h>
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 13;
const byte d6 = 5;
const byte d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String mainMenuOptions[3] = { "Start", "Settings", "About" };
const int mainMenuSize = 3;
int mainMenuIndex = 0;

String settingsMenuOptions[3] = { "LCD brightness", "Mat brightness", "Back" };
const int settingsMenuSize = 3;
int settingsMenuIndex = 0;

bool needUpdate = true;

unsigned long scrollMillis;
const unsigned long scrollDelay = 300;

#define TWO_SECONDS 2000
const int matrixBrightnessModifier = 3;

void setup() {
  lcd.clear();
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);
  //EEPROM.put(MATRIX_BRIGHTNESS_BYTE, matrixBrightness);
  //EEPROM.put(LCD_BRIGHTNESS_BYTE, lcdBrightness);
  EEPROM.get(MATRIX_BRIGHTNESS_BYTE, matrixBrightness);
  EEPROM.get(LCD_BRIGHTNESS_BYTE, lcdBrightness);

  randomSeed(analogRead(seedPin));
  //pinMode(buttonPin, INPUT_PULLUP);
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(playerXPin, INPUT);
  pinMode(playerYPin, INPUT);
  pinMode(bulletXPin, INPUT);
  pinMode(bulletYPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(lcdBrightnessPin, OUTPUT);

  analogWrite(lcdBrightnessPin, lcdBrightness * lcdBrightnessModifier);

  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(THREE, ZERO);
  lcd.print("Welcome to");
  lcd.setCursor(TWO, ONE);
  lcd.print("Blast Radius");
  delay(TWO_SECONDS);
  lcd.clear();
  lc.setIntensity(0, matrixBrightness * matrixBrightnessModifier);
  //startGame();
}
void loop() {
  //lcd.print("test");



  if (running) {  //While the game is running, we update the game and then render it 60 times per second (every frame)
    currentMillis = millis();
    if (currentMillis - previousFrameMillis > frameDelay) {
      lcd.setCursor(0, 0);
      // print the number of seconds since reset:

      update();
      render();
      previousFrameMillis = currentMillis;
    }
  } else {  //If the game is not running, we handle endgame sounds (win/lose) and display a corresponding image on the matrix
    updateEndgameSounds();

    handleMenu();
    //printOptions(mainMenuOptions,mainMenuSize,mainMenuIndex);
    //mainMenuIndex++;

    checkStart();  //We also check if the player wants to restart the game
  }
}

void update() {
  score--;  //The score decreases every second
  updatePlayerPosition();
  updateBullet();
  updateExplosion();
  updateSound();
  updateHud();
  if (checkWin()) {
    Serial.println(score);
    running = false;
    checkHighscore();
    makeWinSound = true;
    soundMillis = millis();
    win = true;
    menuState = ENDGAME;
    displayEndgameMessage();
  }
}

void updatePlayerPosition() {

  int intCurrentX = int(currentX);
  int intCurrentY = int(currentY);
  entityMatrix[intCurrentX][intCurrentY] = EMPTY;

  int xValue = analogRead(playerXPin);  // Read X-axis value
  int yValue = analogRead(playerYPin);  // Read Y-axis value
  //For every direction, we will check if the joystick is in said direction and if the player can move there (not exiting matrix bounds colliding with an enemy or a wall)
  if (xValue > middle + deadZone && entityMatrix[intCurrentX - 1][intCurrentY] != WALL && entityMatrix[intCurrentX - 1][intCurrentY] != ENEMY && intCurrentX - 1 >= 0) {
    currentX -= velocity;
  }
  if (xValue < middle - deadZone && entityMatrix[intCurrentX + 1][intCurrentY] != WALL && entityMatrix[intCurrentX + 1][intCurrentY] != ENEMY && intCurrentX + 1 < matrixSize) {
    currentX += velocity;
  }
  if (yValue > middle + deadZone && entityMatrix[intCurrentX][intCurrentY + 1] != WALL && entityMatrix[intCurrentX][intCurrentY + 1] != ENEMY && intCurrentY + 1 < matrixSize) {
    currentY += velocity;
  }
  if (yValue < middle - deadZone && entityMatrix[intCurrentX][intCurrentY - 1] != WALL && entityMatrix[intCurrentX][intCurrentY - 1] != ENEMY && intCurrentY - 1 >= 0) {
    currentY -= velocity;
  }
  int newIntCurrentX = int(currentX);
  int newIntCurrentY = int(currentY);
  if (newIntCurrentX != intCurrentX || newIntCurrentY != intCurrentY) {  //If the player position changes, update the display matrix
    matrix[intCurrentX][intCurrentY] = false;
    matrix[newIntCurrentX][newIntCurrentY] = true;
  }
  entityMatrix[newIntCurrentX][newIntCurrentY] = PLAYER;
}

void updateBullet() {
  bulletCounter++;
  if (!bulletFired) {                       // If the bullet is not fired (we can only have 1 active bullet at a time), check if the right joystick moves in a direction enough for the bullet to be fired
    float xValue = analogRead(bulletXPin);  // Read X-axis value
    float yValue = analogRead(bulletYPin);  // Read Y-axis value
    //Similar to player movement, but checking every direction at once, to see if we need to initialize the bullet. Also make sure we are not in an explosion animation, and enough time has passed since the last bullet has been fired
    if ((xValue > middle + bulletDeadZone || xValue < middle - bulletDeadZone || yValue > middle + bulletDeadZone || yValue < middle - bulletDeadZone) && bulletCounter > bulletDelay && !explosion) {
      bulletCounter = 0;
      bulletXVel = -1 * (xValue - middle) / BULLET_VELOCITY_MODIFIER;  //*-1 because of matrix orientation
      bulletYVel = (yValue - middle) / BULLET_VELOCITY_MODIFIER;
      bulletFired = true;
      if (xValue > middle + bulletDeadZone) {  //Up
        bulletCurrentX = int(currentX) - 1;
        bulletCurrentY = int(currentY) + HALF;  //+half because we work with floating points, and converting from float to int gives the floor() value of the floating point
      }
      if (xValue < middle - bulletDeadZone) {  //Down
        bulletCurrentX = int(currentX) + 1;
        bulletCurrentY = int(currentY) + HALF;
      }
      if (yValue > middle + bulletDeadZone) {  //Right
        bulletCurrentX = int(currentX) + HALF;
        bulletCurrentY = int(currentY) + 1;
      }
      if (yValue < middle - bulletDeadZone) {  //Left
        bulletCurrentX = int(currentX) + HALF;
        bulletCurrentY = int(currentY) - 1;
      }
      if (entityMatrix[int(bulletCurrentX)][int(bulletCurrentY)] != WALL) {
        if (entityMatrix[int(bulletCurrentX)][int(bulletCurrentY)] == ENEMY) {  // If the bullet would spawn on an enemy, it would override it in the entity matrix, so the explosion would not trigger, so we check for this manually before spawing in the bullet
          explosion = true;
          makeExplosionSound = true;
          explosionX = int(bulletCurrentX);
          explosionY = int(bulletCurrentY);
          explosionCounter = 0;
          entityMatrix[int(bulletCurrentX)][int(bulletCurrentY)] = EMPTY;
          matrix[int(bulletCurrentX)][int(bulletCurrentY)] = false;
          bulletFired = false;
        } else {
          makeLaserSound = true;
          bulletFired = true;
          entityMatrix[int(bulletCurrentX)][int(bulletCurrentY)] = BULLET;
          matrix[int(bulletCurrentX)][int(bulletCurrentY)] = true;
        }
      }
    }

  } else {  //Very similar to player movement, we check if we can move to a new location, and then move there. The only difference is that if we go out of bounds, the bullet despawns, and if we hit an enemy, an explosion occurs
    int intBulletCurrentX = int(bulletCurrentX);
    int intBulletCurrentY = int(bulletCurrentY);
    if (entityMatrix[int(bulletCurrentX)][int(bulletCurrentY)] != WALL) {
      entityMatrix[intBulletCurrentX][intBulletCurrentY] = EMPTY;
    }

    if (bulletCurrentX + bulletXVel < 0 || bulletCurrentX + bulletXVel >= matrixSize || bulletCurrentY + bulletYVel < 0 || bulletCurrentY + bulletYVel >= matrixSize) {
      bulletFired = false;
      matrix[intBulletCurrentX][intBulletCurrentY] = false;
    } else {
      bulletCurrentX += bulletXVel;
      bulletCurrentY += bulletYVel;
      if (int(bulletCurrentX) != intBulletCurrentX || int(bulletCurrentY) != intBulletCurrentY) {  //set prev position to 0 and update the current one
        matrix[intBulletCurrentX][intBulletCurrentY] = false;
        matrix[int(bulletCurrentX)][int(bulletCurrentY)] = true;
      }
      intBulletCurrentX = int(bulletCurrentX);
      intBulletCurrentY = int(bulletCurrentY);



      if (entityMatrix[intBulletCurrentX][intBulletCurrentY] == WALL) {
        bulletFired = false;
      } else {
        if (entityMatrix[intBulletCurrentX][intBulletCurrentY] == ENEMY) {  //HIT
          explosion = true;
          makeExplosionSound = true;
          explosionX = intBulletCurrentX;
          explosionY = intBulletCurrentY;
          explosionCounter = 0;

          //EXPLOSION
          entityMatrix[intBulletCurrentX][intBulletCurrentY] = 0;
          matrix[intBulletCurrentX][intBulletCurrentY] = 0;
          bulletFired = false;


        } else {
          entityMatrix[intBulletCurrentX][intBulletCurrentY] = BULLET;
        }
      }
    }
  }
}


void updateExplosion() {
  //When an explosion occurs, we light up a 3x3 square around the explosion center. After a delay, everything is destroyed. If the player ever enters this 3x3 zone, they die.
  if (explosion) {
    byte multiplier = 1;
    explosionCounter++;
    for (int i = explosionX - 1; i <= explosionX + 1; i++) {
      for (int j = explosionY - 1; j <= explosionY + 1; j++) {
        if (i >= 0 && i < matrixSize && j >= 0 && j < matrixSize) {
          if (explosionCounter < EXPLOSION_DURATION) {
            matrix[i][j] = 1;
            if (entityMatrix[i][j] == PLAYER) {
              //DEATH
              for (int k = 0; k < matrixSize; k++) {
                for (int l = 0; l < matrixSize; l++) {
                  if (entityMatrix[k][l] == ENEMY) {
                    score -= SCORE_LOSS;
                  }
                }
              }
              Serial.print("Your score is ");
              Serial.println(score);
              running = false;
              checkHighscore();
              win = false;
              menuState = ENDGAME;
              displayEndgameMessage();
              makeLoseSound = true;
              soundMillis = millis();
            }
          } else {
            matrix[i][j] = 0;
            if (entityMatrix[i][j] == ENEMY) {
              score += (multiplier * SCORE_PER_KILL);  // This encourages destroying as many walls as possible with only 1 bullet
              multiplier++;
            }
            entityMatrix[i][j] = EMPTY;
            explosion = false;
            //lcd.setCursor(0, 0);
            //lcd.print(score);
          }
        }
      }
    }
  }
}

void updateHud() {
  hudCounter++;
  if (hudCounter == THIRTY_FRAMES) {
    lcd.setCursor(ZERO, ONE);
    lcd.print(score);
    hudCounter = 0;
  }
}

bool checkWin() {  //If there are no more enemies left in the entity matrix, then the player has won
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (entityMatrix[i][j] == ENEMY)
        return false;
    }
  }
  return true;
}

void checkStart() {
  //When both switches are pressed at the same time, the game starts
  //We don't need to debounce these buttons because we don't call this funciton every time the loop is executed, but rather every frame
  bool leftSwitchValue = digitalRead(leftSwitchPin);
  bool rightSwitchValue = digitalRead(rightSwitchPin);
  if (!leftSwitchValue && !rightSwitchValue) {
    startGame();
  }
}

void render() {
  //Here we handle the blinking logic

  ///PLAYER
  playerFrameCounter++;
  bulletFrameCounter++;


  if (playerBlinkState && playerFrameCounter > playerOnInterval) {
    playerFrameCounter = 0;
    matrix[int(currentX)][int(currentY)] = false;
    playerBlinkState = false;
  } else if (!playerBlinkState && playerFrameCounter > playerOffInterval) {
    playerFrameCounter = 0;
    matrix[int(currentX)][int(currentY)] = true;
    playerBlinkState = true;
  }


  ///BULLET
  if (bulletBlinkState && bulletFrameCounter > bulletOnInterval) {
    for (int i = 0; i < matrixSize; i++) {
      for (int j = 0; j < matrixSize; j++) {
        if (entityMatrix[i][j] == BULLET) {
          matrix[i][j] = false;
        }
      }
    }
    bulletFrameCounter = 0;
    bulletBlinkState = false;
  }
  if (!bulletBlinkState && bulletFrameCounter > bulletOffInterval) {
    for (int i = 0; i < matrixSize; i++) {
      for (int j = 0; j < matrixSize; j++) {
        if (entityMatrix[i][j] == BULLET) {
          matrix[i][j] = true;
        }
      }
    }
    bulletFrameCounter = 0;
    bulletBlinkState = true;
  }


  updateMatrix();
}

void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);  // set each led individually
    }
  }
}



void printEntityMatrix() {  //Function for debugging purposes
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Serial.print(entityMatrix[i][j]);
    }
    Serial.println("");
  }
  Serial.println("");
}

void displayImage(uint64_t image) {
  for (int i = 0; i < EIGHT; i++) {
    byte row = (image >> i * matrixSize) & 0xFF;
    for (int j = 0; j < matrixSize; j++) {
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

void startGame() {

  //Reset every value back to its default one, and then start the game

  Serial.print("Current highscore is ");
  EEPROM.get(HIGHSCORE_BYTE, highscore);

  Serial.println(highscore);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Current Score:");
  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // Clear the matrix display
  //updateMatrix();
  //noTone(buzzerPin);
  makeExplosionSound = false;
  makeWinSound = false;
  makeLoseSound = false;
  makeLaserSound = false;
  currentX = initialX;
  currentY = initialY;
  entityMatrix[int(initialX)][int(initialY)] = PLAYER;
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (i != currentX || j != currentY) {
        entityMatrix[i][j] = random(TWO) * ENEMY;  // randomly generate enemies around the room
        matrix[i][j] = entityMatrix[i][j];
      }
    }
  }
  entityMatrix[int(initialX)][int(initialX) - 1] = matrix[int(initialX)][int(initialX) - 1] = false;  //Make sure the player always has one square to move to
  score = initialScore;
  running = true;
  bulletFired = false;
  explosion = false;
  bulletCounter = 0;
  explosionCounter = 0;
  winSoundCounter = 0;
  loseSoundCounter = 0;
  laserSoundCounter = 0;
}

void updateSound() {  //Sound that happens while the game is running, so we work with frames

  if (makeLaserSound) {
    switch (laserSoundCounter) {
      case FRAME_1:
        {
          tone(buzzerPin, FREQUENCY_1000, LASER_SOUND_DURATION);
          break;
        }
      case FRAME_4:
        {
          tone(buzzerPin, FREQUENCY_1500, LASER_SOUND_DURATION);
          makeLaserSound = false;
          laserSoundCounter = 0;
          break;
        }
      default:
        {
          break;
        }
    }
    laserSoundCounter++;
  }
  if (makeExplosionSound) {
    switch (explosionSoundCounter) {
      case FRAME_1:
        {
          tone(buzzerPin, FREQUENCY_900, EXPLOSION_SOUND_DURATION_1);
          break;
        }
      case FRAME_7:
        {
          tone(buzzerPin, FREQUENCY_1200, EXPLOSION_SOUND_DURATION_2);
          break;
        }
      case FRAME_16:
        {
          tone(buzzerPin, FREQUENCY_1000, EXPLOSION_SOUND_DURATION_3);
          makeExplosionSound = false;
          explosionSoundCounter = 0;
          break;
        }
      default:
        {
          break;
        }
    }
    explosionSoundCounter++;
  }
}

void updateEndgameSounds() {  //Sound that happens while the game is running, so we work with millis()
  if (makeWinSound) {
    switch (winSoundCounter) {
      case 0:
        {
          winSoundCounter++;
          tone(buzzerPin, NOTE_C5, WIN_SOUND_DURATION_1);
          break;
        }
      case 1:
        {
          if ((millis() - soundMillis) > WIN_SOUND_DELAY_1) {
            winSoundCounter++;
            tone(buzzerPin, NOTE_D5, WIN_SOUND_DURATION_1);
          }
          break;
        }
      case 2:
        {
          if ((millis() - soundMillis) > WIN_SOUND_DELAY_2) {
            winSoundCounter++;
            tone(buzzerPin, NOTE_E5, WIN_SOUND_DURATION_1);
          }
          break;
        }
      case 3:
        {
          if ((millis() - soundMillis) > WIN_SOUND_DELAY_3) {
            winSoundCounter++;
            tone(buzzerPin, NOTE_G5, WIN_SOUND_DURATION_2);
          }
          break;
        }
      case 4:
        {
          if ((millis() - soundMillis) > WIN_SOUND_DELAY_4) {
            //noTone(buzzerPin);
            makeWinSound = false;

            winSoundCounter = 0;
            needUpdate = true;
          }
          break;
        }
      default:
        {
          break;
        }
    }
  }
  if (makeLoseSound) {
    switch (loseSoundCounter) {
      case 0:
        {
          loseSoundCounter++;
          tone(buzzerPin, NOTE_G4, LOSE_SOUND_DURATION_1);
          break;
        }
      case 1:
        {
          if ((millis() - soundMillis) > LOSE_SOUND_DELAY_1) {
            loseSoundCounter++;
            tone(buzzerPin, NOTE_F4, LOSE_SOUND_DURATION_1);
          }
          break;
        }
      case 2:
        {
          if ((millis() - soundMillis) > LOSE_SOUND_DELAY_2) {
            loseSoundCounter++;
            tone(buzzerPin, NOTE_D4, LOSE_SOUND_DURATION_1);
          }
          break;
        }
      case 3:
        {
          if ((millis() - soundMillis) > LOSE_SOUND_DELAY_3) {
            loseSoundCounter++;
            tone(buzzerPin, NOTE_A3, LOSE_SOUND_DURATION_2);
          }
          break;
        }
      case 4:
        {
          if ((millis() - soundMillis) > LOSE_SOUND_DELAY_4) {
            makeLoseSound = false;
            loseSoundCounter = 0;
            needUpdate = true;
          }
          break;
        }
      default:
        {
          break;
        }
    }
  }
}

void checkHighscore() {  //Utility function for checking if we reach a new highscore when the game ends
  if (score > highscore) {
    Serial.println("NEW NIGHSCORE:");
    Serial.println(score);
    EEPROM.put(HIGHSCORE_BYTE, score);
    highscore = score;
  }
}

bool inMainMenu = false;
bool inSettingMenu = false;

bool upMenuInput = false;
bool downMenuInput = false;
bool leftMenuInput = false;
bool rightMenuInput = false;



#define MAIN_MENU_START 0
#define MAIN_MENU_SETTINGS 1
#define MAIN_MENU_ABOUT 2


#define LCD_BRIGHTNESS 0
#define MAT_BRIGHTNESS 1

void handleMenu() {
  displayMenu();
  handleMenuInput();
}
byte lastSwitchReading;
unsigned long lastSwitchDebounceTime;
byte switchButtonState;
byte switchRequest;

int codeToExecute;


void displayMenu() {
  debounce(leftSwitchPin, lastSwitchReading, lastSwitchDebounceTime, switchButtonState, switchRequest);
  switch (menuState) {
    case MAIN_MENU:
      {
        displayImage(IMAGES[HOME_IMAGE]);
        if (upMenuInput) {
          if (mainMenuIndex > 0) {
            mainMenuIndex--;
            needUpdate = true;
          }
          upMenuInput = false;
        }
        if (downMenuInput) {
          if (mainMenuIndex < mainMenuSize - 1) {
            mainMenuIndex++;
            needUpdate = true;
          }
          downMenuInput = false;
        }
        if (leftMenuInput) {
          leftMenuInput = false;
        }
        if (rightMenuInput) {
          rightMenuInput = false;
        }
        if (needUpdate) {
          tone(buzzerPin, FREQUENCY_1500, LASER_SOUND_DURATION);
          printOptions(mainMenuOptions, mainMenuSize, mainMenuIndex);
          needUpdate = false;
        }

        switch (mainMenuIndex) {
          case MAIN_MENU_START:
            {
              if (switchRequest) {
                switchRequest = false;
                startGame();
              }
              break;
            }
          case MAIN_MENU_SETTINGS:
            {
              if (switchRequest) {
                switchRequest = false;
                needUpdate = true;
                menuState = SETTINGS;

                //Settings menu
              }
              break;
            }
          case MAIN_MENU_ABOUT:
            {
              if (switchRequest) {
                switchRequest = false;
                needUpdate = true;
                menuState = ABOUT;
                lcd.clear();
                lcd.setCursor(FIVE, ZERO);
                lcd.print("About");
              }
              break;
            }
          default:
            {
              break;
            }
        }


        break;
      }
    case SETTINGS:
      {
        displayImage(IMAGES[SETTINGS_IMAGE]);
        if (upMenuInput) {
          if (settingsMenuIndex > 0) {
            settingsMenuIndex--;
            needUpdate = true;
          }
          upMenuInput = false;
        }
        if (downMenuInput) {
          if (settingsMenuIndex < settingsMenuSize - 1) {
            settingsMenuIndex++;
            needUpdate = true;
          }
          downMenuInput = false;
        }
        if (leftMenuInput) {
          leftMenuInput = false;
        }
        if (rightMenuInput) {
          rightMenuInput = false;
        }
        if (needUpdate) {
          tone(buzzerPin, FREQUENCY_1500, LASER_SOUND_DURATION);
          printOptions(settingsMenuOptions, settingsMenuSize, settingsMenuIndex);
          needUpdate = false;
        }
        switch (settingsMenuIndex) {
          case 0:
            {  //lcd brightness
              if (switchRequest) {
                menuState = EXECUTING_CODE;
                codeToExecute = LCD_BRIGHTNESS;
                lcd.clear();
                lcd.setCursor(ONE, ZERO);
                lcd.print("LCD brightness");
                lcd.setCursor(EIGHT, ONE);
                lcd.print(lcdBrightness);
                switchRequest = false;
              }
              break;
            }
          case 1:
            {  //mat brightness
              if (switchRequest) {
                menuState = EXECUTING_CODE;
                codeToExecute = MAT_BRIGHTNESS;
                lcd.clear();
                lcd.setCursor(ONE, ZERO);
                lcd.print("Mat brightness");
                lcd.setCursor(EIGHT, ONE);
                lcd.print(matrixBrightness);
                switchRequest = false;
              }

              break;
            }
          case 2:
            {
              //back
              if (switchRequest) {
                menuState = MAIN_MENU;
                needUpdate = true;
                switchRequest = false;
              }
              break;
            }
        }


        break;
      }
    case ABOUT:
      {
        if (millis() - scrollMillis > scrollDelay) {
          Serial.println("Test");
          if (iCursor == (aboutString2.length() - 1)) {
            iCursor = 0;
          }
          lcd.setCursor(0, 1);
          if (iCursor < aboutString2.length() - numberOfColumns) {
            for (int iChar = iCursor; iChar < iCursor + numberOfColumns; iChar++) {
              lcd.print(aboutString2[iChar]);
            }
          } else {
            for (int iChar = iCursor; iChar < (aboutString2.length() - 1); iChar++) {
              lcd.print(aboutString2[iChar]);
            }

            for (int iChar = 0; iChar <= numberOfColumns - (aboutString2.length() - iCursor); iChar++) {
              lcd.print(aboutString2[iChar]);
            }
          }
          iCursor++;
          scrollMillis = millis();
        }
        if (switchRequest) {
          menuState = MAIN_MENU;
          needUpdate = true;
          switchRequest = false;
        }

        break;
      }
    case EXECUTING_CODE:
      {
        switch (codeToExecute) {
          case LCD_BRIGHTNESS:
            {
              if (needUpdate) {
                tone(buzzerPin, FREQUENCY_1500, LASER_SOUND_DURATION);
                lcd.setCursor(EIGHT, ONE);
                lcd.print(lcdBrightness);
                needUpdate = false;
              }
              if (upMenuInput) {
                if (lcdBrightness < numberOfOptions) {
                  lcdBrightness++;
                  analogWrite(lcdBrightnessPin, lcdBrightness * lcdBrightnessModifier);
                  needUpdate = true;
                }
                upMenuInput = false;
              }
              if (downMenuInput) {
                if (lcdBrightness > 0) {
                  lcdBrightness--;
                  analogWrite(lcdBrightnessPin, lcdBrightness * lcdBrightnessModifier);
                  needUpdate = true;
                }
                downMenuInput = false;
              }
              if (leftMenuInput) {
                if (lcdBrightness > 0) {
                  lcdBrightness--;
                  analogWrite(lcdBrightnessPin, lcdBrightness * lcdBrightnessModifier);
                  needUpdate = true;
                }
                leftMenuInput = false;
              }
              if (rightMenuInput) {
                if (lcdBrightness < numberOfOptions) {
                  lcdBrightness++;
                  analogWrite(lcdBrightnessPin, lcdBrightness * lcdBrightnessModifier);
                  needUpdate = true;
                }
                rightMenuInput = false;
              }

              if (switchRequest) {
                menuState = SETTINGS;
                switchRequest = false;
                needUpdate = true;
                EEPROM.put(LCD_BRIGHTNESS_BYTE, lcdBrightness);
              }
              break;
            }
          case MAT_BRIGHTNESS:
            {
              if (needUpdate) {
                tone(buzzerPin, FREQUENCY_1500, LASER_SOUND_DURATION);
                lcd.setCursor(EIGHT, ONE);
                lcd.print(matrixBrightness);
                needUpdate = false;
              }
              if (upMenuInput) {
                if (matrixBrightness < numberOfOptions) {
                  matrixBrightness++;
                  lc.setIntensity(0, matrixBrightness * matrixBrightnessModifier);
                  needUpdate = true;
                }
                upMenuInput = false;
              }
              if (downMenuInput) {
                if (matrixBrightness > 0) {
                  matrixBrightness--;
                  lc.setIntensity(0, matrixBrightness * matrixBrightnessModifier);
                  needUpdate = true;
                }
                downMenuInput = false;
              }
              if (leftMenuInput) {
                if (matrixBrightness > 0) {
                  matrixBrightness--;
                  lc.setIntensity(0, matrixBrightness * matrixBrightnessModifier);
                  needUpdate = true;
                }
                leftMenuInput = false;
              }
              if (rightMenuInput) {
                if (matrixBrightness < numberOfOptions) {
                  matrixBrightness++;
                  lc.setIntensity(0, matrixBrightness * matrixBrightnessModifier);
                  needUpdate = true;
                }
                rightMenuInput = false;
              }

              if (switchRequest) {
                EEPROM.put(MATRIX_BRIGHTNESS_BYTE, matrixBrightness);
                menuState = SETTINGS;
                switchRequest = false;
                needUpdate = true;
              }
              break;
            }
        }
        break;
      }
    case ENDGAME:
      {
        if (win) {
          displayImage(IMAGES[WIN_IMAGE]);
        } else {
          displayImage(IMAGES[LOSE_IMAGE]);
        }
        if (switchRequest) {
          menuState = MAIN_MENU;
          switchRequest = false;
          needUpdate = true;
        }
      }
  }
}

void printOptions(String options[], int size, int index) {
  lcd.clear();
  if (index < size - 1) {
    lcd.setCursor(0, 0);
    lcd.print(">");
    lcd.setCursor(1, 0);
    lcd.print(options[index]);
    lcd.setCursor(1, 1);
    lcd.print(options[index + 1]);
  } else {
    lcd.setCursor(1, 0);
    lcd.print(options[index - 1]);
    lcd.setCursor(0, 1);
    lcd.print(">");
    lcd.setCursor(1, 1);
    lcd.print(options[index]);
  }
}



void handleMenuInput() {
  int xValue = analogRead(A0);  // Read X-axis value
  int yValue = analogRead(A1);  // Read Y-axis value
  int coordSum = xValue + yValue;
  if ((xValue > middle - deadZone && xValue < middle + deadZone && yValue > middle - deadZone && yValue < middle + deadZone)  //check if we are in the dead zone
      || abs(xValue - yValue) < joystickBuffer                                                                                //buffer for secondary diagonal
      || abs(xValue - yValue) > joystickRange - joystickBuffer) {                                                             //buffer for main diagonal
    /*Serial.print((xValue > middle - deadZone && xValue < middle + deadZone && yValue > middle - deadZone && yValue < middle + deadZone));
    Serial.print(abs(xValue - yValue) < joystickBuffer );
    Serial.println(abs(xValue - yValue) > joystickRange - joystickBuffer);*/
    joystickState = IDLE_STATE;
  } else if (xValue >= yValue && coordSum > joystickRange) {  //if x > y and their sum is greater than 1023, then the joystick is in the UP position
    //UP
    if (joystickState != UP_STATE && millis() - joystickMillis > joystickTimeBuffer) {

      Serial.println("UP");
      joystickState = UP_STATE;
      upMenuInput = true;
      joystickMillis = millis();
      //needUpdate=true;
    }
  } else if (xValue > yValue && coordSum < joystickRange) {  //if x > y and their sum is not greater than 1023, then the joystick is in the LEFT position
    //LEFT
    if (joystickState != LEFT_STATE && millis() - joystickMillis > joystickTimeBuffer) {

      Serial.println("LEFT");
      joystickState = LEFT_STATE;
      leftMenuInput = true;
      joystickMillis = millis();
    }
  } else if (xValue < yValue && coordSum > joystickRange) {  //if x < y and their sum is  greater than 1023, then the joystick is in the RIGHT position
    //RIGHT
    if (joystickState != RIGHT_STATE && millis() - joystickMillis > joystickTimeBuffer) {

      Serial.println("RIGHT");
      joystickState = RIGHT_STATE;
      rightMenuInput = true;
      joystickMillis = millis();
    }
  } else if (xValue < yValue && coordSum < joystickRange) {  //finally, if x < y and their sum is not greater than 1023, then the joystick is in the DOWN position
    //DOWN
    if (joystickState != DOWN_STATE && millis() - joystickMillis > joystickTimeBuffer) {

      Serial.println("DOWN");
      joystickState = DOWN_STATE;
      downMenuInput = true;
      joystickMillis = millis();
      //needUpdate=true;
    }
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


void displayEndgameMessage() {
  lcd.clear();
  lcd.setCursor(THREE, ZERO);
  lcd.print("Final Score");
  lcd.setCursor(SIX, ONE);
  lcd.print(score);
}