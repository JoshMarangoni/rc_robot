
#define IN1 2
#define IN2 3
#define IN3 5
#define IN4 4

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Serial.println("Motor Driver Start");
}

void loop() {
  Serial.println("LEFT");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  delay(2000);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(2000);
  Serial.println("RIGHT");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  delay(2000);
}
