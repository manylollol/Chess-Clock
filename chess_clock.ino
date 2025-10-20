#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD (I2C, PCF8574)
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

// Pins
#define BUTTON_WHITE 2
#define BUTTON_BLACK 3
#define ENCODER_WHITE_A 4
#define ENCODER_WHITE_B 5
#define ENCODER_BLACK_A 6
#define ENCODER_BLACK_B 7
#define LED_WHITE 8
#define LED_BLACK 9
#define BUZZER_WHITE 10
#define BUZZER_BLACK 11

// Timer settings
unsigned long whiteTime = 5 * 60 * 1000; // 5 min in ms
unsigned long blackTime = 5 * 60 * 1000;
unsigned long lastUpdate = 0;

bool whiteTurn = true;
bool running = false;

// Button states
bool lastWhiteButton = HIGH;
bool lastBlackButton = HIGH;

void setup() {
  pinMode(BUTTON_WHITE, INPUT_PULLUP);
  pinMode(BUTTON_BLACK, INPUT_PULLUP);
  pinMode(LED_WHITE, OUTPUT);
  pinMode(LED_BLACK, OUTPUT);
  pinMode(BUZZER_WHITE, OUTPUT);
  pinMode(BUZZER_BLACK, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Chess Clock");
  delay(1000);
  lcd.clear();
  updateDisplay();
}

void loop() {
  unsigned long now = millis();
  // Update timers every second
  if (running && now - lastUpdate >= 1000) {
    lastUpdate = now;
    if (whiteTurn && whiteTime > 0) whiteTime -= 1000;
    else if (!whiteTurn && blackTime > 0) blackTime -= 1000;
    checkBuzzer();
    updateDisplay();
  }

  // Check buttons
  bool whiteBtn = digitalRead(BUTTON_WHITE);
  bool blackBtn = digitalRead(BUTTON_BLACK);

  if (whiteBtn == LOW && lastWhiteButton == HIGH) { // pressed
    whiteTurn = true;
    running = true;
    updateLEDs();
  }
  if (blackBtn == LOW && lastBlackButton == HIGH) { // pressed
    whiteTurn = false;
    running = true;
    updateLEDs();
  }

  lastWhiteButton = whiteBtn;
  lastBlackButton = blackBtn;
}

// Display update
void updateDisplay() {
  lcd.setCursor(0, 0);
  lcd.print("W:");
  lcd.print(formatTime(whiteTime));
  lcd.print("  B:");
  lcd.print(formatTime(blackTime));
}

// Format time as MM:SS
String formatTime(unsigned long t) {
  unsigned long totalSec = t / 1000;
  int min = totalSec / 60;
  int sec = totalSec % 60;
  char buf[6];
  sprintf(buf, "%02d:%02d", min, sec);
  return String(buf);
}

// Update LEDs
void updateLEDs() {
  digitalWrite(LED_WHITE, whiteTurn ? HIGH : LOW);
  digitalWrite(LED_BLACK, whiteTurn ? LOW : HIGH);
}

// Check buzzer for low time (<1 min)
void checkBuzzer() {
  if (whiteTime <= 60000 && whiteTurn) {
    tone(BUZZER_WHITE, 1000, 500); // beep 0.5s
  }
  if (blackTime <= 60000 && !whiteTurn) {
    tone(BUZZER_BLACK, 1000, 500);
  }
}
