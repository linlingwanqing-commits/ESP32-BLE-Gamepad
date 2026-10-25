#include <BleGamepad.h>

BleGamepad bleGamepad("PC Gamepad", "ESP32", 100);

// ===== 左摇杆 =====
#define JOY_LX   4
#define JOY_LY   5
#define JOY_LSW  6

// ===== 右摇杆 =====
#define JOY_RX   7
#define JOY_RY   8
#define JOY_RSW  9

// ===== 扳机 =====
#define TRIG_LT  10
#define TRIG_RT  11

// ===== 标准按键 =====
#define BTN_A    12
#define BTN_B    13
#define BTN_X    14
#define BTN_Y    15
#define BTN_LB   16
#define BTN_RB   17
#define BTN_VIEW 18
#define BTN_MENU 21

// ===== 方向键 =====
#define DPAD_UP    1
#define DPAD_DOWN  2
#define DPAD_LEFT  42
#define DPAD_RIGHT 41

// ===== 扩展按键 M1~M10 =====
#define BTN_M1  38
#define BTN_M2  39
#define BTN_M3  40
#define BTN_M4  47
#define BTN_M5  48
#define BTN_M6  35
#define BTN_M7  36
#define BTN_M8 37

// ===== 校准相关 =====
int lxCenter, lyCenter, rxCenter, ryCenter;
const int CALIB_SAMPLES = 50;

void calibrateSticks() {
  long sumLX = 0, sumLY = 0, sumRX = 0, sumRY = 0;

  delay(500); // 给用户松手的时间

  for (int i = 0; i < CALIB_SAMPLES; i++) {
    sumLX += analogRead(JOY_LX);
    sumLY += analogRead(JOY_LY);
    sumRX += analogRead(JOY_RX);
    sumRY += analogRead(JOY_RY);
    delay(10);
  }

  lxCenter = sumLX / CALIB_SAMPLES;
  lyCenter = sumLY / CALIB_SAMPLES;
  rxCenter = sumRX / CALIB_SAMPLES;
  ryCenter = sumRY / CALIB_SAMPLES;

  Serial.print("Calibration: LX="); Serial.print(lxCenter);
  Serial.print(" LY="); Serial.print(lyCenter);
  Serial.print(" RX="); Serial.print(rxCenter);
  Serial.print(" RY="); Serial.println(ryCenter);
}

void setup() {
  Serial.begin(115200);

  // 所有按键/摇杆按下引脚启用上拉
  pinMode(JOY_LSW, INPUT_PULLUP);
  pinMode(JOY_RSW, INPUT_PULLUP);
  pinMode(BTN_A, INPUT_PULLUP);
  pinMode(BTN_B, INPUT_PULLUP);
  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);
  pinMode(BTN_LB, INPUT_PULLUP);
  pinMode(BTN_RB, INPUT_PULLUP);
  pinMode(BTN_VIEW, INPUT_PULLUP);
  pinMode(BTN_MENU, INPUT_PULLUP);
  pinMode(DPAD_UP, INPUT_PULLUP);
  pinMode(DPAD_DOWN, INPUT_PULLUP);
  pinMode(DPAD_LEFT, INPUT_PULLUP);
  pinMode(DPAD_RIGHT, INPUT_PULLUP);
  pinMode(BTN_M1, INPUT_PULLUP);
  pinMode(BTN_M2, INPUT_PULLUP);
  pinMode(BTN_M3, INPUT_PULLUP);
  pinMode(BTN_M4, INPUT_PULLUP);
  pinMode(BTN_M5, INPUT_PULLUP);
  pinMode(BTN_M6, INPUT_PULLUP);
  pinMode(BTN_M7, INPUT_PULLUP);
  pinMode(BTN_M8, INPUT_PULLUP);
  pinMode(BTN_M9, INPUT_PULLUP);
  pinMode(BTN_M10, INPUT_PULLUP);

  calibrateSticks(); // 开机自动校准

  BleGamepadConfiguration config;
  config.setButtonCount(20);          // 10 标准 + 10 扩展
  config.setWhichAxes(true, true, true, true, true, true, false, false);
  config.setHatSwitchCount(1);

  bleGamepad.begin(&config);
}

void loop() {
  if (!bleGamepad.isConnected()) return;

  // ===== 摇杆（以校准中点为基准双向映射） =====
  int lxRaw = analogRead(JOY_LX);
  int lyRaw = analogRead(JOY_LY);
  int rxRaw = analogRead(JOY_RX);
  int ryRaw = analogRead(JOY_RY);

  int lx = map(lxRaw, lxCenter - 2048, lxCenter + 2047, 0, 32767);
  int ly = map(lyRaw, lyCenter - 2048, lyCenter + 2047, 0, 32767);
  int rx = map(rxRaw, rxCenter - 2048, rxCenter + 2047, 0, 32767);
  int ry = map(ryRaw, ryCenter - 2048, ryCenter + 2047, 0, 32767);

  lx = constrain(lx, 0, 32767);
  ly = constrain(ly, 0, 32767);
  rx = constrain(rx, 0, 32767);
  ry = constrain(ry, 0, 32767);

  bleGamepad.setLeftThumb(lx, ly);
  bleGamepad.setRightThumb(rx, ry);

  // ===== 扳机（霍尔扳机单向映射） =====
  int lt = map(analogRead(TRIG_LT), 0, 4095, 0, 32767);
  int rt = map(analogRead(TRIG_RT), 0, 4095, 0, 32767);
  bleGamepad.setLeftTrigger(constrain(lt, 0, 32767));
  bleGamepad.setRightTrigger(constrain(rt, 0, 32767));

  // ===== 标准按键 =====
  if (digitalRead(BTN_A) == LOW)    bleGamepad.press(BUTTON_1);  else bleGamepad.release(BUTTON_1);
  if (digitalRead(BTN_B) == LOW)    bleGamepad.press(BUTTON_2);  else bleGamepad.release(BUTTON_2);
  if (digitalRead(BTN_X) == LOW)    bleGamepad.press(BUTTON_3);  else bleGamepad.release(BUTTON_3);
  if (digitalRead(BTN_Y) == LOW)    bleGamepad.press(BUTTON_4);  else bleGamepad.release(BUTTON_4);
  if (digitalRead(BTN_LB) == LOW)   bleGamepad.press(BUTTON_5);  else bleGamepad.release(BUTTON_5);
  if (digitalRead(BTN_RB) == LOW)   bleGamepad.press(BUTTON_6);  else bleGamepad.release(BUTTON_6);
  if (digitalRead(BTN_VIEW) == LOW) bleGamepad.press(BUTTON_7);  else bleGamepad.release(BUTTON_7);
  if (digitalRead(BTN_MENU) == LOW) bleGamepad.press(BUTTON_8);  else bleGamepad.release(BUTTON_8);
  if (digitalRead(JOY_LSW) == LOW)  bleGamepad.press(BUTTON_9);  else bleGamepad.release(BUTTON_9);
  if (digitalRead(JOY_RSW) == LOW)  bleGamepad.press(BUTTON_10); else bleGamepad.release(BUTTON_10);

  // ===== 扩展按键 M1~M10 =====
  if (digitalRead(BTN_M1) == LOW)  bleGamepad.press(BUTTON_11); else bleGamepad.release(BUTTON_11);
  if (digitalRead(BTN_M2) == LOW)  bleGamepad.press(BUTTON_12); else bleGamepad.release(BUTTON_12);
  if (digitalRead(BTN_M3) == LOW)  bleGamepad.press(BUTTON_13); else bleGamepad.release(BUTTON_13);
  if (digitalRead(BTN_M4) == LOW)  bleGamepad.press(BUTTON_14); else bleGamepad.release(BUTTON_14);
  if (digitalRead(BTN_M5) == LOW)  bleGamepad.press(BUTTON_15); else bleGamepad.release(BUTTON_15);
  if (digitalRead(BTN_M6) == LOW)  bleGamepad.press(BUTTON_16); else bleGamepad.release(BUTTON_16);
  if (digitalRead(BTN_M7) == LOW)  bleGamepad.press(BUTTON_17); else bleGamepad.release(BUTTON_17);
  if (digitalRead(BTN_M8) == LOW)  bleGamepad.press(BUTTON_18); else bleGamepad.release(BUTTON_18);
  if (digitalRead(BTN_M9) == LOW)  bleGamepad.press(BUTTON_19); else bleGamepad.release(BUTTON_19);
  if (digitalRead(BTN_M10) == LOW) bleGamepad.press(BUTTON_20); else bleGamepad.release(BUTTON_20);

  // ===== 方向键 =====
  bool up    = (digitalRead(DPAD_UP) == LOW);
  bool down  = (digitalRead(DPAD_DOWN) == LOW);
  bool left  = (digitalRead(DPAD_LEFT) == LOW);
  bool right = (digitalRead(DPAD_RIGHT) == LOW);

  int hat = HAT_CENTERED;
  if (up && left)         hat = HAT_UP_LEFT;
  else if (up && right)   hat = HAT_UP_RIGHT;
  else if (down && left)  hat = HAT_DOWN_LEFT;
  else if (down && right) hat = HAT_DOWN_RIGHT;
  else if (up)            hat = HAT_UP;
  else if (down)          hat = HAT_DOWN;
  else if (left)          hat = HAT_LEFT;
  else if (right)         hat = HAT_RIGHT;

  bleGamepad.setHat1(hat);

  delay(10);
}