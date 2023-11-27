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
const int buttonPin = 6;
const int leftSwitchPin = 6;
const int rightSwitchPin = 7;
const int buzzerPin = 9;
const int seedPin = 3;  //A pin that is not connected to anything, used as a seed for the random function

const float initialX = 5;  //Initial player position
const float initialY = 5;
const int initialScore = 1000;

//Frame control logic
const int frameDelay = 1000 / 60;  // 60 FPS
unsigned long previousFrameMillis;
unsigned long currentMillis;

//Joystick logic
const float velocity = 0.064;
const int deadZone = 250;
const int bulletDeadZone = 250;
const int middle = 512;

bool running = true;
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


LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);  // Create an LedControl object to manage the LED matrix
byte matrixBrightness = 2;                                 // Variable to set the brightness level of the matrix

float currentX = 5;
float currentY = 5;

long score = 1000;
long highscore = 0;
#define HIGHSCORE_BYTE 0

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

#define LOSE_IMAGE 0
#define WIN_IMAGE 1

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



const uint64_t IMAGES[] = {
  0x3e67e7bfbfe7673e,  //Lose
  0x60a888700c15110e   //Win

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

void setup() {
  randomSeed(analogRead(seedPin));
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(leftSwitchPin, INPUT_PULLUP);
  pinMode(rightSwitchPin, INPUT_PULLUP);
  Serial.begin(9600);
  pinMode(playerXPin, INPUT);
  pinMode(playerYPin, INPUT);
  pinMode(bulletXPin, INPUT);
  pinMode(bulletYPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  startGame();
}
void loop() {
  if (running) {  //While the game is running, we update the game and then render it 60 times per second (every frame)
    currentMillis = millis();
    if (currentMillis - previousFrameMillis > frameDelay) {
      update();
      render();
      previousFrameMillis = currentMillis;
    }
  } else {  //If the game is not running, we handle endgame sounds (win/lose) and display a corresponding image on the matrix
    updateEndgameSounds();
    if (win) {
      displayImage(IMAGES[WIN_IMAGE]);
    } else {
      displayImage(IMAGES[LOSE_IMAGE]);
    }
    checkStart();  //We also check if the player wants to restart the game
  }
}

void update() {
  score--;  //The score decreases every second
  updatePlayerPosition();
  updateBullet();
  updateExplosion();
  updateSound();
  if (checkWin()) {
    Serial.println(score);
    running = false;
    checkHighscore();
    makeWinSound = true;
    soundMillis = millis();
    win = true;
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
          }
        }
      }
    }
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
      lc.setLed(0, col, row, matrix[row][col]);  // set each led individually
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
  for (int i = 0; i < 8; i++) {
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

  lc.shutdown(0, false);                 // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness);  // sets brightness (0~15 possible values)
  lc.clearDisplay(0);                    // Clear the matrix display
  //updateMatrix();
  noTone(buzzerPin);
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