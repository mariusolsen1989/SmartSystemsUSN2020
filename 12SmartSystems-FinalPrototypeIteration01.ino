// ITERATION #1 - 29.11.2020
// 1. ADDED LINE FOLLOWER FUNCTIONALITY (TESTET OK - 29.11.2020).
// 2. ADDED 180 DEGREES TURN WHEN DETECTING DOUBLE BLACK LINE (TESTET OK - 29.11.2020).
// 3. ADDED FULL STOP (POWER SAVING) WHEN RETURNING TO INITIAL POSITION (TESTET OK - 29.11.2020).
// 4. ADDED ULTRASONIC SENSOR CODE (TESTET OK - 29.11.2020).

/* EACH ENTRY IN THE ARRAY REPRESENTS ONE PLANT */
bool hasBeenWatered = false;

/* CHECKS IF THE PLANT IS CURRENTLY BEING WATERED */
bool isBeingWatered;

/* ULTRASONIC OUTPUTS */
const int triggerPin = 24;
const int echoPin = 26;

/* ULTRASONIC READINGS */
long duration;
int distance;

/* LEFT MOTOR INPUTS */
int LM = 2;
int LM_IN1 = 3;
int LM_IN2 = 4;

/* RIGHT MOTOR INPUTS */
int RM = 7;
int RM_IN3 = 5;
int RM_IN4 = 6;

/* DEFINES MAXIMUM MOTOR SPEED. THIS CAN GO UP TO 255. WILL BE USING 60 UNDER TESTING */
int maxSpeed = 100;

/* IR-SENSOR READINGS. LS = LEFT SIDE | RS = RIGHT SIDE */
int LS_PIN = A0;
int RS_PIN = A1;
int LS_STATE;
int RS_STATE;

/* COUNTER TO SEE HOW MANY TIMES DOUBLE BLACK LINES HAVE ACCURED */
int blackLineCounter;

void setup() 
{
  /* SET THE 'triggerPin' AS OUTPUT AND THE 'echoPin' AS INPUT */
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  /* ALL MOTOR CONNECTIONS ARE SET TO OUTPUT */
  pinMode(LM, OUTPUT);
  pinMode(RM, OUTPUT);
  pinMode(LM_IN1, OUTPUT);
  pinMode(LM_IN2, OUTPUT);
  pinMode(RM_IN3, OUTPUT);
  pinMode(RM_IN4, OUTPUT);
  
  /* GEAR MOTORS ARE SET TO OFF IN THE INITIAL STATE */
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);

  blackLineCounter = 0;

  Serial.begin(9600);
}

void loop() 
{
  /* UPDATING LEFT AND RIGHT SENSOR STATES CONTINOUSLY */
  LineFollowerUpdate();

  /* CLEARING THE 'triggerPin' */
  TriggerPinClear();

  /* SET THE 'triggerPin' to HIGH STATE FOR ABOUT 10 MICRO SECONDS AND THEN CLEARS IT */
  TriggerHighLowClear();

  /* READS THE 'echoPin' BEFORE RETURNING THE SOUND WAVE TRAVEL TIME IN MICRO SECONDS */
  duration = pulseIn(echoPin, HIGH);

  /* USED TO CALCULATE THE DISTANCE OF THE OBJECT RECIEVED FROM THE ULTRASONIC SENSOR */
  distance= duration*0.034/2;

  /* PRINTS THE DISTANCE ON THE MONITOR - JUST FOR TESTING PURPOSES */
  PrintUltrasonicDistance();

  /* FOR PLANT NUMBER 1 */
  if ((!hasBeenWatered) && (distance <= 30))
  {
    delay(100);
    isBeingWatered = true;
  
    while (isBeingWatered)
    {
      StopVehicleObsticle();
      
      /* SET THE TRIGGERPIN TO LOW TO BE ABLE TO TURN */
      TriggerPinClear();
      
      // VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE!
      TurnVehicleLeftReverse();
      delay(1500);
      TurnVehicleRight();
      delay(1500);

      /* STOPPING THE VEHICLE  */
      StopVehicleObsticle();

      /* ADD CRANE AND WATERING OPERATIONS HERE - ADDING 5 SECONDS DELAY JUST FOR TESTING */
      delay(5000);

      /* VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE! */
      TurnVehicleLeft();
      TurnVehicleRightReverse();
      delay(1000);

      TriggerHighLowClear();
      isBeingWatered = false;
    }

    hasBeenWatered = true;
  }
  
  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD MOVE FORWARD. */
  else if(!(digitalRead(LS_PIN)) && !(digitalRead(RS_PIN)))
  {
    MoveVehicleForward(maxSpeed);
  }

  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD TURN TO THE RIGHT. */
  else if(!digitalRead(LS_PIN) && digitalRead(RS_PIN))
  {
    TurnVehicleRight();
  }

  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD TURN TO THE LEFT. */
  else if(digitalRead(LS_PIN) && (!digitalRead(RS_PIN)))
  {
    TurnVehicleLeft();
  }

  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD STOP. */
  else if(digitalRead(LS_PIN) && digitalRead(RS_PIN))
  {
    StopVehicleBlackLines(maxSpeed);
  }
}

void PrintUltrasonicDistance()
{
  Serial.print("Distance: ");
  Serial.println(distance);
}

void TriggerPinClear()
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
}

void TriggerHighLowClear()
{
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
}

void LineFollowerUpdate()
{
  LS_STATE = digitalRead(LS_PIN);
  RS_STATE = digitalRead(RS_PIN);
}

/* FUNCTION TO STOP THE VEHICLE BECAUSE OF OBSTICLES */
void StopVehicleObsticle()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

/* FUNCTION TO STOP THE VEHICLE BECAUSE OF DOUBLE BLACK LINES */
void StopVehicleBlackLines(int maxSpeed)
{
  blackLineCounter++;
  StopVehicleObsticle();

  TriggerPinClear();
  
  if (blackLineCounter < 2)
  {
    // ----- The vehicle should turn left or right by 180 degrees ------

    TurnVehicleTest();
    delay(2700);
    
    /* THE VEHICLE WILL MOVE FORWARD AFTER TURNING */
    TriggerHighLowClear();
    
    StopVehicleObsticle();
    delay(500);
    MoveVehicleForward(maxSpeed);
  }
}

/* FUNCTION TO MAKE THE VEHICLE MOVE FORWARDS */
void MoveVehicleForward(int maxSpeed)
{
  analogWrite(LM, maxSpeed);
  analogWrite(RM, maxSpeed);
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

/* FUNCTION TO MAKE THE VEHICLE MOVE TO THE LEFT */
void TurnVehicleLeft()
{
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

/* FUNCTION TO MAKE THE VEHICLE MOVE TO THE LEFT IN REVERSE */
void TurnVehicleLeftReverse()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, HIGH);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

/* FUNCTION TO MAKE THE VEHICLE MOVE TO THE RIGHT */
void TurnVehicleRight()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

/* FUNCTION TO MAKE THE VEHICLE MOVE TO THE RIGHT IN REVERSE */
void TurnVehicleRightReverse()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, HIGH);
}

void TurnVehicleTest()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, HIGH);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}
