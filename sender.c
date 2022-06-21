int LED = 12;
int LIVE = 13;

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(LIVE, OUTPUT);
}

void sendDigit (int number) {
  int level = 32;

  for(int i=0; i<5; i++) {
    level /= 2;
    if(level <=number){
      Serial.print("1");
      number -=level;
      digitalWrite(LED, 1);
    } else {
      Serial.print("0");
      digitalWrite(LED, 0);
    }
    delay(100);
  }
  Serial.println("");
}

void loop() {
  if(Serial.available() <= 0) return;

  digitalWrite(LIVE,1);
  while (Serial.available()) {
    int input = Serial.read() - 64;
    Serial.println(input);
    sendDigit(input);
  }
  digitalWrite(LED, 0);
  digitalWrite(LIVE, 0);
}