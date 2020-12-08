// ITERATION #3 - 06.11.2020
// 1. Fixed problems with servo motor - Tested OK.
// 2. Fixed problems with water pump - Testet OK.
// 3. Re-did the code since there was some minor bugs in it.
// 4. Added servo functionality (it will now go into the soil and read the soil moisture).
// 5. Added water pump activation functionality.
// 6. System test partly OK - Need to: 1. HAVE A LARGER PLANT POT | 2. MAKE ADJUSTMENTS TO THE TURNING.

/* LIBRARIES */
#include "LowPower.h"
#include <Servo.h>

/* VARIABLES NEEDED FOR THE POWER SAVINGS FUNCTION */
int powerSavingHours = 12;
int sleepMax = 450 * powerSavingHours;

/* SERVO MOTOR OBJECT  */
Servo servoCraneOperation;

/* WATER PUMP INPUT AND BOOLEAN FLAG */
int waterPump = 28;
bool pumpActivated = true;

/* EACH ENTRY IN THE ARRAY REPRESENTS ONE PLANT */
bool plantHasBeenWatered = false;

/* CHECKS IF THE PLANT IS CURRENTLY BEING WATERED */
bool plantIsBeingWatered;

/* SOIL MOISTURE SENSOR PIN */
#define sensorPin A2

/* SOIL MOISTURE READINGS */
float soilSensorValue = 0;

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
  /* SERVO ATTACHED TO PIN NUMBER 30 */
  servoCraneOperation.attach(9);

  /* WATERPUMP SET TO OUTPUT & INITIALLY SET IT TO LOW */
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
  
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
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  /* SET THE 'triggerPin' to HIGH STATE FOR ABOUT 10 MICRO SECONDS AND THEN CLEARS IT */
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  /* READS THE 'echoPin' BEFORE RETURNING THE SOUND WAVE TRAVEL TIME IN MICRO SECONDS */
  duration = pulseIn(echoPin, HIGH);

  /* USED TO CALCULATE THE DISTANCE OF THE OBJECT RECIEVED FROM THE ULTRASONIC SENSOR */
  distance = duration*0.034/2;

  /* PRINTS THE DISTANCE ON THE MONITOR - JUST FOR TESTING PURPOSES */
  Serial.print("Distance: ");
  Serial.println(distance);

  /* IF THE PLANT HASN'T BEEN WATERED BEFORE AND THE DISTANCE VARIABLE FROM THE ULTRASONIC SENSOR EVALUATES TO TRUE - THE VEHICLE SHOULD STOP. */
  if ((distance <= 20) && (!plantHasBeenWatered))
  {
    /* ADDING A SMALL DELAY BEFORE STOPPING THE VEHICLE. */
    delay(200);

    /* CHANGING 'isBeingWatered' VARIABLE TO TRUE TO ENTER THE WHILE LOOP */
    plantIsBeingWatered = true;
    
    while (plantIsBeingWatered)
    {
      StopVehicleObsticle();

      /* SET THE TRIGGERPIN TO LOW TO BE ABLE TO TURN */
      digitalWrite(triggerPin, LOW);
      delayMicroseconds(2);

      // VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE!
      RotateVehicleLeftDegrees();
      delay(1600);

      /* STOPPING THE VEHICLE  */
      StopVehicleObsticle();

      delay(1000);
      
      /* CODE FOR SERVO MOTOR (GOING DOWN) SHOULD BE ADDED HERE. */ 
      ServoMotorRotateDown();
      /* CODE FOR SOIL MOISTURE SENSOR SHOULD BE ADDED HERE. */

      for (int i = 0; i < 10; i++)
      {
        SoilMoistureSensorReadings();
        /* CODE FOR PUMP ACTIVATION SHOULD BE ADDED HERE */
        if ((soilSensorValue < 800) && (plantIsBeingWatered))
        {
          WaterPumpActivation();
          
          /* CHANGING 'isBeingWatered' VARIABLE TO FALSE TO EXIT THE WHILE LOOP */
          plantIsBeingWatered = false;
        }
      }
      
      /* CHANGING 'isBeingWatered' VARIABLE TO FALSE TO EXIT THE WHILE LOOP */
      plantIsBeingWatered = false;
      ServoMotorRotateUp();
      
    }
       delay(2000);    
       ServoMotorRotateUp();

       delay(1000);

       RotateVehicleRightDegrees();
       delay(1600);

       StopVehicleObsticle();

       digitalWrite(triggerPin, HIGH);
       delayMicroseconds(10);
       digitalWrite(triggerPin, LOW);
       
       /* SET THE VARIABLE TO TRUE SO THAT THE SYSTEM WILL KNOW THAT THIS PLANT HAS BEEN GIVEN WATER RECENTLY. */
       plantHasBeenWatered = true;
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

  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  if (blackLineCounter < 2)
  {
    // ----- The vehicle should turn left or right by 180 degrees ------

    RotateVehicleRightDegrees();
    /* USING 2.5 SECONDS TO TURN 180 DEGREES */
    delay(2500);
    
    /* THE VEHICLE WILL MOVE FORWARD AFTER TURNING */
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);

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

void SoilMoistureSensorReadings()
{
  for (int i = 0; i <= 100; i++)
  {
    soilSensorValue = soilSensorValue + analogRead(sensorPin);
    delay(1);
  }
  soilSensorValue = soilSensorValue / 100.0;
  Serial.println(soilSensorValue);
  delay(20);
}

/* CRANE MOVES DOWN */
void ServoMotorRotateDown()
{
  servoCraneOperation.writeMicroseconds (700);
}

/* CRANE MOVES UP AGAN */
void ServoMotorRotateUp()
{
  servoCraneOperation.writeMicroseconds (1400);
}

/* ACTIVATION OF WATER PUMP */
void WaterPumpActivation()
{
  digitalWrite(waterPump, LOW);
  delay(3000);
  digitalWrite(waterPump, HIGH);
  delay(1000);
}
