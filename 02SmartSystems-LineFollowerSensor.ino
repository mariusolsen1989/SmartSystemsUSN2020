/*-------definning Inputs------*/
// Motor A (Left Motor Connections)
int LM = 2;
int LM_IN1 = 3;
int LM_IN2 = 4;

// Motor B (Right Motor Connections)
int RM = 7;
int RM_IN3 = 5;
int RM_IN4 = 6;

// Define maximum motor speed. This can go up to 255. Using 50 under testing.
int MaxSpeed = 130;

// Sensor Reading
int LS_PIN = A0;
int RS_PIN = A1;
int LS_STATE;
int RS_STATE;

void setup() {
  // Set all the motor control pins to outputs
  pinMode(LM, OUTPUT);
  pinMode(RM, OUTPUT);
  pinMode(LM_IN1, OUTPUT);
  pinMode(LM_IN2, OUTPUT);
  pinMode(RM_IN3, OUTPUT);
  pinMode(RM_IN4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);

  Serial.begin(9600);
}

void loop() {

  LS_STATE = digitalRead(LS_PIN);
  RS_STATE = digitalRead(RS_PIN);
  
Serial.print(LS_STATE);
Serial.print(" -- ");
Serial.print(RS_STATE);
Serial.println("");
Serial.println("");
delay(100);

  if(!(digitalRead(LS_PIN)) && !(digitalRead(RS_PIN)))                              // Move forward
  {
        analogWrite(LM, MaxSpeed);
        analogWrite(RM, MaxSpeed);
        digitalWrite(LM_IN1, HIGH);
        digitalWrite(LM_IN2, LOW);
        digitalWrite(RM_IN3, HIGH);
        digitalWrite(RM_IN4, LOW);
  }

  else if(!digitalRead(LS_PIN) && digitalRead(RS_PIN))                             // Turn right
  {
        digitalWrite(LM_IN1, LOW);
        digitalWrite(LM_IN2, LOW);
        digitalWrite(RM_IN3, HIGH);
        digitalWrite(RM_IN4, LOW);
        delay(300);
  }

  else if(digitalRead(LS_PIN) && (!digitalRead(RS_PIN)))                            // Turn left
  {
        digitalWrite(LM_IN1, HIGH);
        digitalWrite(LM_IN2, LOW);
        digitalWrite(RM_IN3, LOW);
        digitalWrite(RM_IN4, LOW);
  }

  else if(digitalRead(LS_PIN) && digitalRead(RS_PIN))                        // STOP
  {
        digitalWrite(LM_IN1, LOW);
        digitalWrite(LM_IN2, LOW);
        digitalWrite(RM_IN3, LOW);
        digitalWrite(RM_IN4, LOW);
  }
}
