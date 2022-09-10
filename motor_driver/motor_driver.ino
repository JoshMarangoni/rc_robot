
#define IN1 3
#define IN2 2
#define IN3 5
#define IN4 4

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
  Serial.println("Motor Driver Start");
}

void loop() {
  off();
  delay(2000);
  straight();
  delay(2000);
  left();
  delay(2000);
  straight();
  delay(2000);
  off();
  delay(2000);
  right();
  delay(2000);
}
