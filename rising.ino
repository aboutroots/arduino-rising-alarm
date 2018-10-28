#include <DS3231.h>
#include <LedControl.h>

// ALARM SETTINGS
// This is the start time of the brightening period, so remember to set it 
// *fadeTime* before waking up.
int alarm_h = 5;
int alarm_m = 30; 
int fadeTime = 15; // in minutes.

// LED SETTINGS
const int pin_uled = 9;

// RTC SETTINGS
DS3231 rtc(SDA, SCL);
Time time;
const long ONE_SECOND = 1000;
const long ONE_MINUTE = ONE_SECOND * 60;

// BUTTONS SETTINGS
const int pin_get = 2;
int button_get_state = 0;

// DISPLAY SETTINGS ------------------------------
// numbers matrix for led
const uint64_t IMAGES[] = {
  0x1c2222222222221c,
  0x1c08080808080c08,
  0x3e0408102020221c,
  0x1c2220201820221c,
  0x20203e2224283020,
  0x1c2220201e02023e,
  0x1c2222221e02221c,
  0x040404081020203e,
  0x1c2222221c22221c,
  0x1c22203c2222221c
};
const uint64_t ALARM_IMAGE = 0x6666667e66663c00; // letter A
const uint64_t TIME_IMAGE = 0x18181818185a7e00; // letter T
const int IMAGES_LEN = sizeof(IMAGES)/8;
const int pin_DIN = 12;
const int pin_CS = 11;
const int pin_CLK = 10;

LedControl lc = LedControl(pin_DIN, pin_CLK, pin_CS, 0);

void displayImage(uint64_t image){
  for(int i = 0; i<8; i++){
    byte row = (image >> i * 8) & 0xFF;
    for(int j = 0; j<8; j++){
      lc.setLed(0, i, j, bitRead(row, j));
    }
  }
}

void displayTime(int h, int m){
  // display hour
  displayImage(IMAGES[int(h / 10)]);
  delay(500);
  lc.clearDisplay(0);
  delay(100);
  displayImage(IMAGES[h % 10]);
  delay(500);
  lc.clearDisplay(0);
  delay(1000);

  // display minutes
  displayImage(IMAGES[int(m / 10)]);
  delay(500);
  lc.clearDisplay(0);
  delay(100);
  displayImage(IMAGES[m % 10]);
  delay(500);
  lc.clearDisplay(0);
}

void setup() {
  //SETUP LED
  pinMode(pin_uled, OUTPUT);
  analogWrite(pin_uled, 0);

  //SETUP BUTTON
  pinMode(pin_get, INPUT); 

  //SETUP CLOCK
  rtc.begin();
  time = rtc.getTime();

  //SETUP DISPLAY
  lc.shutdown(0, false);
  lc.setIntensity(0,40);
  lc.clearDisplay(0);

  Serial.begin(9600);
}


void loop() {
  button_get_state = digitalRead(pin_get);
  if(button_get_state == HIGH) {
    printTimeToSerial();
    displayImage(ALARM_IMAGE);
    delay(300);
    displayTime(alarm_h, alarm_m);
    delay(500);
    displayImage(TIME_IMAGE);
    delay(300);
    displayTime(time.hour, time.min);
    lc.clearDisplay(0);
    // test_led(); only for debugging purposes
  }

  if (time.hour == alarm_h && time.min >= alarm_m && time.min < alarm_m + 1){
      active();
  }

  time = rtc.getTime();
  Serial.println(rtc.getTimeStr());
  delay(1000);
}

void printTimeToSerial(){
  Serial.println("Checking the time...");
  Serial.print("Alarm: ");
  Serial.print(alarm_h);
  Serial.print(":");
  Serial.print(alarm_m);
  Serial.print(" Current: ");
  Serial.print(time.hour);
  Serial.print(":");
  Serial.print(time.min);
  Serial.print("\n");
}

void test_led(){
  for (int i = 0; i <= 255; i++) {
    analogWrite(pin_uled, i);
    delay(10);
    Serial.print(" led : ");
    Serial.println(i);
  }
  for (int i = 255; i > 0; i--) {
    analogWrite(pin_uled, i);
    delay(10);
    Serial.print(" led : ");
    Serial.println(i);
  }
  analogWrite(pin_uled, 0);
}

void active() {
  Serial.println("led turned on.");

  float delay_time = fadeTime * ONE_MINUTE / 255.0;
  float x;
  int y;
  for (int i = 0; i!=255; i++){
    x = i / 255.0;
    y = x * x * 255;
    Serial.println(y);
    analogWrite(pin_uled, y);
    delay(delay_time);
  }

  // keep the light on for 3 minutes
  Serial.println("Waiting with LED ON");
  delay(ONE_MINUTE * 3);
  analogWrite(pin_uled, 0);
}
