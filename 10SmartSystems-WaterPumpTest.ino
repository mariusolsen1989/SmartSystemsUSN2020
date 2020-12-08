 int waterPump = 28;

bool pumpActivated = true;

void setup() 
{
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
}

void loop() 
{
  if (pumpActivated == true)
  {
    digitalWrite(waterPump, LOW);
    delay(2000);
    digitalWrite(waterPump, HIGH);
    delay(1000);
    pumpActivated = false;
  }
}
