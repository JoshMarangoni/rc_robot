
#define M1 19
#define M2 23

void setup()
{
    Serial.begin(115200);
    pinMode(M1, OUTPUT);
    pinMode(M2, OUTPUT);
}

void loop()
{
  digitalWrite(M1, HIGH);
  digitalWrite(M2, LOW);

  delay(1000);

  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);

  delay(1000);

  digitalWrite(M1, LOW);
  digitalWrite(M2, HIGH);

  delay(1000);

  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);

  delay(1000);
}
