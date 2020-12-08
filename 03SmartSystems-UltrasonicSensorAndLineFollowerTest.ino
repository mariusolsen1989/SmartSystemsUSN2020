// DEFINES ULTRASONIC OUTPUTS
const int triggerPin = 24;
const int echoPin = 26;

// defines variables for Ultrasonic Sensor
long duration;
int distance;

/*-------definning Inputs------*/
// MOTOR A (LEFT MOTOR CONNECTIONS)
int LM = 2;
int LM_IN1 = 3;
int LM_IN2 = 4;

// MOTOR B (RIGHT MOTOR CONNECTIONS)
int RM = 7;
int RM_IN3 = 5;
int RM_IN4 = 6;

// DEFINES MAXIMUM MOTOR SPEED. THIS CAN GO UP TO 255. WILL BE USING 80 UNDER TESTING.
int MaxSpeed = 100;

// Sensor Reading
int LS_PIN = A0;
int RS_PIN = A1;
int LS_STATE;
int RS_STATE;

// CHECKS IF THE PLANT HAS BEEN WATERED BEFORE.
bool hasBeenWatered = false;


void setup() 
{
  pinMode(triggerPin, OUTPUT);                                              // Sets the triggerPin as OUTPUT
  pinMode(echoPin, INPUT);                                                  // Sets the echoPin as INPUT
  
  // SET ALL THE MOTOR CONTROL PINS TO OUTPUTS
  pinMode(LM, OUTPUT);
  pinMode(RM, OUTPUT);
  pinMode(LM_IN1, OUTPUT);
  pinMode(LM_IN2, OUTPUT);
  pinMode(RM_IN3, OUTPUT);
  pinMode(RM_IN4, OUTPUT);
  
  // TURN OFF MOTORS - INITIAL STATE
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);

  Serial.begin(9600);
}

void loop() {

  LS_STATE = digitalRead(LS_PIN);
  RS_STATE = digitalRead(RS_PIN);

  // Clears the trigPin
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance= duration*0.034/2;
  
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);

  if (!hasBeenWatered && distance < 10)                                                                // Just testing with distance to the plants
  {
    delay(100);
    StopVehicle();
    TurnVehicleRight();
    delay(1000);
    StopVehicle();
    delay(5000);
    
    hasBeenWatered = true;
    // START SERVOMOTOR (ROTATE 90 DEGREES).
    // 
  }

  if(!(digitalRead(LS_PIN)) && !(digitalRead(RS_PIN)))                              // Move forward
  {
    MoveVehicleForward(MaxSpeed);
  }

  else if(!digitalRead(LS_PIN) && digitalRead(RS_PIN))                              // Turn right
  {
    TurnVehicleRight();
  }

  else if(digitalRead(LS_PIN) && (!digitalRead(RS_PIN)))                            // Turn left
  {
    TurnVehicleLeft();
  }

  else if(digitalRead(LS_PIN) && digitalRead(RS_PIN))                               // STOP
  {
    StopVehicle();
  }
}

void StopVehicle()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

void MoveVehicleForward(int maxSpeed)
{
  analogWrite(LM, maxSpeed);
  analogWrite(RM, maxSpeed);
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

void TurnVehicleLeft()
{
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

void TurnVehicleRight()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}
