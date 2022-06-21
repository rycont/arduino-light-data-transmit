#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

int THRESHOLD_LIVE = 600;
int DATAPORT_LIVE = 1300;

int IS_LIVE = A0;
int DATA_PORT = A1;


void setup() {
  pinMode(IS_LIVE, INPUT);
  pinMode(DATA_PORT, INPUT);
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.print("ASDF");
}

int digitsToInt(int digits[]) {
  return digits[0] * 16 + digits[1] * 8 + digits[2] * 4 + digits[3] * 2 + digits[4];
}

void loop() {
  
  if(analogRead(IS_LIVE) <= THRESHOLD_LIVE) return;
  int charpoint = 0;
  lcd.clear();
  while(analogRead(IS_LIVE) > THRESHOLD_LIVE) {
    int digits[] = { -1, -1, -1, -1, -1 };

    for(int i = 0; i < 5; i++) {
      delay(10);
      int data = analogRead(DATA_PORT);
      delay(20);
      data += analogRead(DATA_PORT);
      delay(20);
      data += analogRead(DATA_PORT);
      delay(20);
      data += analogRead(DATA_PORT);
      delay(20);
      data += analogRead(DATA_PORT);
      delay(9);
     
      digits[i] = data > DATAPORT_LIVE;
    }
    

    char res = (char(digitsToInt(digits) + 64));
    if(res == '@') continue;
 
    lcd.setCursor(charpoint % 16, charpoint / 16);
    charpoint++;
    lcd.print(res);
  }

  Serial.println("Input Done!");
}