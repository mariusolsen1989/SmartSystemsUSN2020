// ITERATION #2 - 29.11.2020
// 1. ADDED POWER SLEEPING FUNCTIONALITY.
// 2. ADDED TURNING 90 DEGREES FUNCTIONALITY WHEN VEHICLE IS WITHIN 30 CM FROM AN OBJECT.
// 3. TESTING DO BE DONE 30.11.2020
// 4. TESTING OK (30.11.2020)

/* LIBRARIES */
#include "LowPower.h"

/* VARIABLES NEEDED FOR THE POWER SAVINGS FUNCTION */
int powerSavingHours = 12;
int sleepMax = 450 * powerSavingHours;

/* EACH ENTRY IN THE ARRAY REPRESENTS ONE PLANT */
bool hasBeenWatered[1] = {false};

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
  distance = duration*0.034/2;

  /* PRINTS THE DISTANCE ON THE MONITOR - JUST FOR TESTING PURPOSES */
  Serial.print("Distance: ");
  Serial.println(distance);

  /* FOR PLANT NUMBER 1 */
  if (distance <= 10 && !hasBeenWatered[0])
  {
    WateringProcedure();
  }
  
  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD MOVE FORWARD. */
  if(!(digitalRead(LS_PIN)) && !(digitalRead(RS_PIN)))
  {
    MoveVehicleForward(maxSpeed);
  }

  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD TURN TO THE RIGHT. */
  if(!digitalRead(LS_PIN) && digitalRead(RS_PIN))
  {
    TurnVehicleRight();
  }

  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD TURN TO THE LEFT. */
  if(digitalRead(LS_PIN) && (!digitalRead(RS_PIN)))
  {
    TurnVehicleLeft();
  }

  /* IF THE STATEMENT EVALUATES TO TRUE - THAN THE VEHICLE SHOULD STOP. */
  if(digitalRead(LS_PIN) && digitalRead(RS_PIN))
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

    RotateVehicleRightDegrees();
    /* USING 2.5 SECONDS TO TURN 180 DEGREES */
    delay(2500);
    
    /* THE VEHICLE WILL MOVE FORWARD AFTER TURNING */
    TriggerHighLowClear();

    /* VEHICLE SHOULD STOP AFTER 180 DEGREES ROTATION AND WAIT FOR 0.5 SECONDS BEFORE CONTINUING */
    StopVehicleObsticle();
    delay(500);
    MoveVehicleForward(maxSpeed);
  }
  else
  {
    /* GOING INTO POWER SAVING MODE TO REDUCE POWER USAGE */
    for (int i = 0; i < sleepMax; i++)
    {
      PowerSavingsMode();
    }
    /* RESETING THE COUNTER AFTER POWER SAVING MODE IS COMPLETED */
    blackLineCounter = 0;
    
    RotateVehicleRightDegrees();
    /* USING 2.5 SECONDS TO TURN 180 DEGREES */
    delay(2500);
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

void RotateVehicleRightDegrees()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, HIGH);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

void RotateVehicleLeftDegrees()
{
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, HIGH);
}

/* PUT THE ARDUINO TO SLEEP FOR 8 SECONDS */
void PowerSavingsMode()
{
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

void WateringProcedure()
{
  delay(200);
  isBeingWatered = true;
  
  while (isBeingWatered)
  {
    StopVehicleObsticle();
    
    /* SET THE TRIGGERPIN TO LOW TO BE ABLE TO TURN */
    TriggerPinClear();
      
    // VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE!
    RotateVehicleLeftDegrees();
    delay(1400);

    /* STOPPING THE VEHICLE  */
    StopVehicleObsticle();
      
    delay(5000);

    /* ADD CRANE AND WATERING OPERATIONS HERE - ADDING 5 SECONDS DELAY JUST FOR TESTING */
    // Rotate Crane Down
    // Perform soil moisture readings
    // Activate pump IF values are LESS than 900
    // ELSE - Rotate crane UP AGAIN

    /* VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE! */
    RotateVehicleRightDegrees();
    delay(1000);

    TriggerHighLowClear();
    isBeingWatered = false;
   }
   
    hasBeenWatered[0] = true;
}
