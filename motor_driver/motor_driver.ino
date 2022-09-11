
#define IN1 18
#define IN2 19
#define IN3 22
#define IN4 23
#define LED 32

void straight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
}

void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, HIGH);
}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN4, HIGH);
  digitalWrite(IN3, LOW);
}

void off() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN4, LOW);
  digitalWrite(IN3, LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.println("Motor Driver Start");
}

void loop() {
  off();
  delay(1000);
  straight();
  delay(1000);
  off();
  delay(1000);
  left();
  delay(1000);
  off();
  delay(1000);
  straight();
  delay(1000);
  off();
  delay(1000);
  right();
  delay(1000);
}
