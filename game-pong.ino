#include <Arduino_LED_Matrix.h>
#include <SevSeg.h>

ArduinoLEDMatrix matrix;
SevSeg sevSeg;

const int SCREEN_HEIGHT = 8;
const int SCREEN_WIDTH = 12;

const unsigned long TIME_CAP = 30000;

byte screen[SCREEN_HEIGHT][SCREEN_WIDTH] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

unsigned long last = 0;

double player = SCREEN_WIDTH / 2 - 1;

int ballX = SCREEN_WIDTH / 2 - 1;
int ballY = SCREEN_HEIGHT / 2;
int direction = 0;

int score = 0;

bool alive = true;

void setup() {
  matrix.begin();

  byte digitPins[] = { 10, 11, 12, 13 };
  byte segmentPins[] = { 9, 2, 3, 5, 6, 8, 7, 4 };
  sevSeg.begin(COMMON_CATHODE, 4, digitPins, segmentPins, true);
  sevSeg.setBrightness(100);
}

void loop() {
  double joystick = round(((analogRead(0) + 1) / (1024.0 / 2) - 1) * 10) / 10.0;
  player += joystick / 500;
  player = max(player, 1);
  player = min(player, SCREEN_WIDTH - 2);

  unsigned long time = millis();
  if (time < last) {
    last = 0;
  }

  unsigned long interval = max(10, (TIME_CAP - time) / (TIME_CAP / 250));
  if (alive && time - last >= interval) {
    last = time;
    switch (direction) {
      case 0:
        ballX--;
        ballY--;
        if (ballX <= 0 && ballY <= 0) {
          direction = 2;
          score++;
        } else if (ballX <= 0) {
          direction = 1;
          score++;
        } else if (ballY <= 0) {
          direction = 3;
          score++;
        }
        break;
      case 1:
        ballX++;
        ballY--;
        if (ballX >= SCREEN_WIDTH - 1 && ballY <= 0) {
          direction = 3;
          score++;
        } else if (ballX >= SCREEN_WIDTH - 1) {
          direction = 0;
          score++;
        } else if (ballY <= 0) {
          direction = 2;
          score++;
        }
        break;
      case 2:
        ballX++;
        ballY++;
        if (ballX >= SCREEN_WIDTH - 1 && ballY >= SCREEN_HEIGHT - 1) {
          direction = 0;
          score++;
        } else if (ballX >= SCREEN_WIDTH - 1) {
          direction = 3;
          score++;
        } else if (ballY >= SCREEN_HEIGHT - 1) {
          direction = 1;
          score++;
        }
        if (ballY == SCREEN_HEIGHT - 3 && abs((double)ballX - round(player)) <= 1.5) {
          direction = 1;
        }
        break;
      case 3:
        ballX--;
        ballY++;
        if (ballX <= 0 && ballY >= SCREEN_HEIGHT - 1) {
          direction = 1;
          score++;
        } else if (ballX <= 0) {
          direction = 2;
          score++;
        } else if (ballY >= SCREEN_HEIGHT - 1) {
          direction = 0;
          score++;
        }
        if (ballY == SCREEN_HEIGHT - 3 && abs((double)ballX - round(player)) <= 1.5) {
          direction = 0;
        }
        break;
    }
    ballX = max(ballX, 0);
    ballX = min(ballX, SCREEN_WIDTH - 1);
    ballY = max(ballY, 0);
    ballY = min(ballY, SCREEN_HEIGHT - 1);
  }

  if (ballY >= SCREEN_HEIGHT - 1) {
    alive = false;
  }

  for (int i = 0; i < SCREEN_HEIGHT; i++) {
    for (int j = 0; j < SCREEN_WIDTH; j++) {
      screen[i][j] = alive ? 0 : 1;
    }
  }
  for (int i = 0; i < 3; i++) {
    int index = round(player) - 1 + i;
    if (index >= 0 && index <= SCREEN_WIDTH - 1) {
      screen[SCREEN_HEIGHT - 2][index] = 1;
    }
  }
  screen[ballY][ballX] = 1;

  matrix.renderBitmap(screen, 8, 12);

  sevSeg.setNumber(score);
  sevSeg.refreshDisplay();
}