#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BTN_FLAP 8
#define LED1 A0
#define LED2 A1
#define LED3 A2
#define SW1 4
#define SW2 5
#define SW3 6

int birdY = 32;
int birdVelocity = 0;
int gravity = 1;
int flapStrength = -5;

int pipeX = SCREEN_WIDTH;
int pipeGapY = 24;
int pipeWidth = 15;
int gapHeight = 25;
int score = 0;
bool gameOver = false;
bool gameWon = false;
bool playedRick = false;
bool transitionToDino = false;
bool blinkBopDone = false;
bool flappyDone = false;
bool flappyRetry = false;

int dinoX = 5;
int dinoY = 48;
bool dinoJumping = false;
int dinoJumpVelocity = 0;
int dinoGravity = 1;
int dinoScore = 0;
int obstacleX = 128;
bool dinoGame = false;
bool dinoOver = false;
bool dinoWon = false;
bool dinoLevelHard = false;
bool dinoRetry = false;
bool rickDisplayed = false;

const int patternLength = 5;
int pattern[patternLength];
int userInput[patternLength];

void setup() {
  pinMode(BTN_FLAP, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  showIntro();
}

void loop() {
  if (!blinkBopDone) {
    playBlinkBop();
  } else if (!flappyDone) {
    if (!gameOver) playFlappyPixel();
    else {
      showFailMessage();
      delay(2000);
      resetFlappy();
    }
  } else if (!dinoOver && !dinoWon) {
    playDinoGame();
  } else if (dinoOver && !dinoWon) {
    showFailMessage();
    delay(2000);
    resetDino();
  } else if (dinoWon && !rickDisplayed) {
    showFinalMessage();
  }
}

void showIntro() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("HEY BUDDY...");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LETS PLAY");
  display.setCursor(0, 30);
  display.println("SOME GAMES");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("AND EARN");
  display.setCursor(0, 30);
  display.println("SOME AURA");
  display.display();
  delay(2000);
}

void playBlinkBop() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("MATCH LED PATTERN");
  display.display();
  delay(1000);

  for (int i = 0; i < patternLength; i++) {
    pattern[i] = random(0, 3);
    lightUp(pattern[i]);
    delay(300);
    allOff();
    delay(200);
  }

  for (int i = 0; i < patternLength; i++) {
    int btn = waitForButton();
    userInput[i] = btn;
    lightUp(btn);
    delay(200);
    allOff();
  }

  for (int i = 0; i < patternLength; i++) {
    if (userInput[i] != pattern[i]) {
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println("WRONG PATTERN!");
      display.display();
      delay(2000);
      return;
    }
  }

  display.clearDisplay();
  display.setCursor(0, 20);
  display.println("GOOD JOB!");
  display.display();
  delay(2000);
  blinkBopDone = true;
}

void lightUp(int ledIndex) {
  digitalWrite(LED1, ledIndex == 0);
  digitalWrite(LED2, ledIndex == 1);
  digitalWrite(LED3, ledIndex == 2);
}

void allOff() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
}

int waitForButton() {
  while (true) {
    if (digitalRead(SW1) == LOW) return 0;
    if (digitalRead(SW2) == LOW) return 1;
    if (digitalRead(SW3) == LOW) return 2;
  }
}

void playFlappyPixel() {
  display.clearDisplay();
  if (digitalRead(BTN_FLAP) == LOW) birdVelocity = flapStrength;
  birdVelocity += gravity;
  birdY += birdVelocity;

  if (birdY < 0 || birdY > SCREEN_HEIGHT) {
    gameOver = true;
    return;
  }

  pipeX -= 2;
  if (pipeX < -pipeWidth) {
    pipeX = SCREEN_WIDTH;
    pipeGapY = random(10, SCREEN_HEIGHT - gapHeight - 10);
    score++;
  }

  if (birdY < pipeGapY || birdY > pipeGapY + gapHeight) {
    if (pipeX < 10 && pipeX + pipeWidth > 0) {
      gameOver = true;
      return;
    }
  }

  if (score >= 6) {
    flappyDone = true;
    return;
  }

  if (score >= 4 && score < 5) display.invertDisplay(true);
  else display.invertDisplay(false);

  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  display.fillRect(pipeX, 0, pipeWidth, pipeGapY, WHITE);
  display.fillRect(pipeX, pipeGapY + gapHeight, pipeWidth, SCREEN_HEIGHT, WHITE);
  display.fillRect(10, birdY, 5, 5, WHITE);
  display.setCursor(100, 0);
  display.setTextSize(1);
  display.print("Score:");
  display.print(score);
  display.display();
  delay(60);
}

void resetFlappy() {
  birdY = 32;
  birdVelocity = 0;
  score = 0;
  pipeX = SCREEN_WIDTH;
  gameOver = false;
}

void playDinoGame() {
  display.clearDisplay();
  if (dinoJumping) {
    dinoY += dinoJumpVelocity;
    dinoJumpVelocity += dinoGravity;
    if (dinoY >= 48) {
      dinoY = 48;
      dinoJumping = false;
    }
  } else if (digitalRead(BTN_FLAP) == LOW) {
    dinoJumping = true;
    dinoJumpVelocity = -7;
  }

  obstacleX -= dinoLevelHard ? 8 : 6;
  if (obstacleX < 0) {
    obstacleX = SCREEN_WIDTH;
    dinoScore++;
    if (dinoScore == 5) dinoLevelHard = true;
  }

  if (obstacleX < dinoX + 10 && obstacleX > dinoX && dinoY > 40) {
    dinoOver = true;
    return;
  }

  if (dinoScore >= 10) {
    dinoWon = true;
    return;
  }

  display.fillRect(dinoX, dinoY, 10, 10, WHITE);
  display.fillRect(obstacleX, 50, 8, 8, WHITE);
  if (dinoLevelHard) display.fillRect(obstacleX, 10, 8, 8, WHITE);
  display.setCursor(90, 0);
  display.setTextSize(1);
  display.print("Score:");
  display.print(dinoScore);
  display.display();
  delay(25);
}

void resetDino() {
  dinoY = 48;
  dinoJumping = false;
  dinoJumpVelocity = 0;
  dinoScore = 0;
  dinoLevelHard = false;
  obstacleX = SCREEN_WIDTH;
  dinoOver = false;
}

void showFinalMessage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("YOU WON BRO..");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LETS SING A SONG");
  display.setCursor(0, 10);
  display.println("TOGETHER...");
  display.display();
  delay(2000);

  for (int i = 5; i > 0; i--) {
    display.clearDisplay();
    display.setCursor(50, 25);
    display.setTextSize(2);
    display.print(i);
    display.display();
    delay(1000);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Never gonna give you up");
  display.setCursor(0, 10);
  display.println("Never gonna let you down");
  display.setCursor(0, 20);
  display.println("Never gonna run around");
  display.setCursor(0, 30);
  display.println("And desert you!");
  display.display();
  rickDisplayed = true;
  while (1);
}

void showFailMessage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(10, 20);
  if (score < 3 || dinoOver) display.println("HAHA..YOU LOOSE");
  else display.println("NICE..TRY ONCE MORE");
  display.display();
}
