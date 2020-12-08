// ITERATION #4 - 02.11.2020
// 1. Fixed a bug on the ultrasonic sensor were readings were not accurate.
// 2. Changed GROUND, 5V and PinNumber on the servo motor since this is causing problems with the other components.
// 3. 

/* LIBRARIES */
#include "LowPower.h"
#include <Servo.h>

/* VARIABLES NEEDED FOR THE POWER SAVINGS FUNCTION */
int powerSavingHours = 12;
int sleepMax = 450 * powerSavingHours;

/* SERVO MOTOR OBJECT  */
Servo servoCraneOperation;

// DEFINES ULTRASONIC OUTPUTS
const int triggerPin = 24;
const int echoPin = 26;

// defines variables for Ultrasonic Sensor
long duration;
int distance;

/* EACH ENTRY IN THE ARRAY REPRESENTS ONE PLANT */
bool hasBeenWatered = false;

/* CHECKS IF THE PLANT IS CURRENTLY BEING WATERED */
bool isBeingWatered;

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

/* COUNTER TO SEE HOW MANY TIMES DOUBLE BLACK LINES HAVE ACCURED */
int blackLineCounter;

/* SOIL MOISTURE SENSOR PIN */
#define sensorPin A2

/* SOIL MOISTURE READINGS */
float soilSensorValue = 0;


void setup() 
{
  /* SERVO ATTACHED TO PIN NUMBER 30 */
  servoCraneOperation.attach(9);
  
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

  blackLineCounter = 0;

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

  if ((distance <= 20) && (!hasBeenWatered))                                                                // Just testing with distance to the plants
  {
        delay(150);
    isBeingWatered = true;
  
  while (isBeingWatered)
  {
    StopVehicleObsticle();
    
    /* SET THE TRIGGERPIN TO LOW TO BE ABLE TO TURN */
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
      
    // VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE!
    RotateVehicleLeftDegrees();
    delay(1400);

    /* STOPPING THE VEHICLE  */
    StopVehicleObsticle();
      
    delay(5000);

    ServoMotorRotateDown();

    for (int i = 0; i < 10; i++)
    {
      SoilMoistureSensorReadings();
      
      /* PUMP ACTIVATION */
      if ((soilSensorValue < 900) && (isBeingWatered))
      {
        // pumpActivated = true;
        
        // WaterPumpActivation();
            
        // pumpActivated = false;
        
        /* CHANGING 'isBeingWatered' VARIABLE TO FALSE TO EXIT THE WHILE LOOP */
        isBeingWatered = false;
        
        ServoMotorRotateUp();
        
        /* SET THE VARIABLE TO TRUE SO THAT THE SYSTEM WILL KNOW THAT THIS PLANT HAS BEEN GIVEN WATER RECENTLY. */
        hasBeenWatered = true;
        
       }
       else
       {
         ServoMotorRotateUp();
       }
     }

     RotateVehicleRightDegrees();
     delay(1400);
     StopVehicleObsticle();
    
     digitalWrite(triggerPin, HIGH);
     delayMicroseconds(10);
     digitalWrite(triggerPin, LOW);
     
     isBeingWatered = false;
   }
     hasBeenWatered = true;
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
    StopVehicleBlackLines(MaxSpeed);
  }
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
void StopVehicleBlackLines(int MaxSpeed)
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
    MoveVehicleForward(MaxSpeed);
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

void MoveVehicleForward(int MaxSpeed)
{
  analogWrite(LM, MaxSpeed);
  analogWrite(RM, MaxSpeed);
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
  servoCraneOperation.writeMicroseconds (900);
}

/* CRANE MOVES UP AGAN */
void ServoMotorRotateUp()
{
  servoCraneOperation.writeMicroseconds (1800);
}
