// ITERATION #4 - 08.12.2020
// 1. Restructuring the code
// 2. Adding more comments on the code
// 3. Added watering procedure for two plants

/* Libraries used */
#include "LowPower.h"
#include <Servo.h>

/* VARIABLE | Power saving feature */
int powerSavingHours = 12;
int sleepMax = 450 * powerSavingHours;

/* VARIABLE | Servo motor object  */
Servo servoCraneOperation;

/* INPUT | Water pump*/
int waterPump = 28;

/* VARIABLE | Checks if the pump is currently activated */
bool pumpActivated = true;

/* VARIABLE | Checks if the plant have been waterered recently */
bool plantHasBeenWatered[2] = {false, false};

/* VARIABLE | Checks if the plant is currently being watered */
bool plantIsBeingWatered;

/* INPUT | Soil moisture sensor */
#define sensorPin A2

/* VARIABLE | Soil moisture sensor readings */
float soilSensorValue = 0;

/* INPUT | Ultrasonic sensor */
const int triggerPin = 24;
const int echoPin = 26;

/* VARIABLE | Ultrasonic sensor */
long duration;
int distance;

/* INPUT | Left motor */
int LM = 2;
int LM_IN1 = 3;
int LM_IN2 = 4;

/* INPUT | Right motor */
int RM = 7;
int RM_IN3 = 5;
int RM_IN4 = 6;

/* VARIABLE | Gear motor speed - 100 seems fine during testing */
int maxSpeed = 150;

/* VARIABLE | Readings from the line follower sensors (LS = Left | RS = Right) */
int LS_PIN = A0;
int RS_PIN = A1;
int LS_STATE;
int RS_STATE;

/* VARIABLE | How many times double black lines have accured */
int blackLineCounter;

void setup() 
{
  /* INITIALIZING | Attaching servo object to pin 9  */
  servoCraneOperation.attach(9);

  /* INITIALIZING | Water pump output - Initially setting it to HIGH */
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);
  
  /* INITIALIZING | Ultrasonic sensor */
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  /* INITIALIZING | Gear motor */
  pinMode(LM, OUTPUT);
  pinMode(RM, OUTPUT);
  pinMode(LM_IN1, OUTPUT);
  pinMode(LM_IN2, OUTPUT);
  pinMode(RM_IN3, OUTPUT);
  pinMode(RM_IN4, OUTPUT);
  
  /* INITIALIZING | Gear motors set to low */
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);

  
  /* INITIALIZING | Black line counter */
  blackLineCounter = 0;

  Serial.begin(9600);
}

void loop() 
{
  /*  */
  /* LINE FOLLOWING SENSORS | Being updated continously */
  LineFollowerUpdate();

  /* ULTRASONIC SENSOR | Clearing triggerpin */
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  /* ULTRASONIC SENSOR | Resetting the triggerpin */
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  /* ULTRASONIC SENSOR | Returning sound wave travel time in micro seconds */
  duration = pulseIn(echoPin, HIGH);

  /* ULTRASONIC SENSOR | Distance calculations to get it into centimeters */
  distance = duration*0.034/2;

  /* ULTRASONIC SENSOR | Printing the distance (for testing purposes) */
  Serial.print("Distance: ");
  Serial.println(distance);

  /* PLANT WATERING PROCEDURE STATEMENT | For plant #1 */
  if ((distance <= 20) && (!plantHasBeenWatered[0]))
  {
    /* DELAY | Adding a small delay before stopping the vehicle */
    delay(400);

    /* VARIABLE | Changing to true to be able to enter the while-loop */
    plantIsBeingWatered = true;

    /* LOOP | Should run as long as the plant is being watered */
    while (plantIsBeingWatered)
    {
      /* VEHICLE FUNCTIONS | Stopping the vehicle */
      StopVehicleObsticle();

      /* VARIABLE | Clearing the triggerpin */
      digitalWrite(triggerPin, LOW);
      delayMicroseconds(2);

      /* VEHICLE FUNCTIONS | Turning vehicle by approximately 90 degrees */
      RotateVehicleLeftDegrees();
      delay(1500);

      /* VEHICLE FUNCTIONS | Stopping the vehicle */
      StopVehicleObsticle();

      /* DELAY | Adding a small delay before the servo motor is starting */
      delay(1000);
      
      /* SERVO MOTOR | Rotate the crane down to the flowerpot */ 
      ServoMotorRotateDown();

      /* LOOP | The soil moisture sensor should perform 10 readings */
      for (int i = 0; i < 10; i++)
      {
        /* SOIL MOISTURE SENSOR | Getting readings from the soil */
        SoilMoistureSensorReadings();

        /* WATER PUMP PROCEDURE | Should only be activated if the soil is dry */
        if ((soilSensorValue < 800) && (plantIsBeingWatered))
        {
          WaterPumpActivation();
          
          /* VARIABLE | Changing it to false to exit the while-loop */
          plantIsBeingWatered = false;
        }
      }
      
      /* VARIABLE | Changing it to false to exit the while-loop */
      plantIsBeingWatered = false;

      /* SERVO MOTOR | Rotate the crane up from the flowerpot */
      ServoMotorRotateUp();
      
    }

      /* DELAY | Adding a small delay before the crane is rotated up again  */
      delay(2000);

      /* SERVO MOTOR | Rotate the crane up from the flowerpot */
      ServoMotorRotateUp();

      /* DELAY | Small delay before turning the vehicle */
      delay(1000);

      /* VEHICLE FUNCTIONS | Turning the vehicle back 90 degrees */
      RotateVehicleRightDegrees();
      delay(2100);

      /* VEHICLE FUNCTIONS | Stopping the vehicle before continuing using the line follower sensors */
      StopVehicleObsticle();

      /* ULTRASONIC SENSOR | Resetting the ultrasonic sensor pins */
      digitalWrite(triggerPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(triggerPin, LOW);

      /* VARIABLE | Set the variable to true to exit the watering procedure */
      plantHasBeenWatered[0] = true;
  }
  
  /* STATEMENT | The vehicle should stop */
  if(!(digitalRead(LS_PIN)) && !(digitalRead(RS_PIN)))
  {
    MoveVehicleForward(maxSpeed);
  }

  /* STATEMENT | The vehicle should turn to the right */
  if(!digitalRead(LS_PIN) && digitalRead(RS_PIN))
  {
    TurnVehicleRight();
  }

  /* STATEMENT | The vehicle should turn to the left */
  if(digitalRead(LS_PIN) && (!digitalRead(RS_PIN)))
  {
    TurnVehicleLeft();
  }

  /* STATEMENT | The vehicle should stop because of identifying double black lines */
  if(digitalRead(LS_PIN) && digitalRead(RS_PIN))
  {
    StopVehicleBlackLines(maxSpeed);
  }
}

/* ULTRASONIC SENSOR FUNCTION | Printing the distance in centimeters */
void PrintUltrasonicDistance()
{
  Serial.print("Distance: ");
  Serial.println(distance);
}

/* LINE FOLLOWER SENSOR FUNCTION | Reading the input from the pins */
void LineFollowerUpdate()
{
  LS_STATE = digitalRead(LS_PIN);
  RS_STATE = digitalRead(RS_PIN);
}

/* GEAR MOTOR FUNCTION | Stopping the vehicle due to an obsticle */
void StopVehicleObsticle()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

/* GEAR MOTOR FUNCTION | Stopping the vehicle due to double black lines */
void StopVehicleBlackLines(int maxSpeed)
{
  /* VARIABLE | Increasing the counter by one */
  blackLineCounter++;
  
  /* VEHICLE FUNCTION | Stopping the vehicle */
  StopVehicleObsticle();

  /* ULTRASONIC SENSOR | Clearing the sensor pins */
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  /* STATEMENT | The vehicle should go turn 180 degrees and continue the watering procedure */
  if (blackLineCounter < 2)
  {
    /* VEHICLE FUNCTION | The vehicle should turn right by 180 degrees */
    RotateVehicleRightDegrees();
    
    /* DELAY | Using a delay to turn the vehicle as we don't have sensors to do it */
    delay(4500);
    
    /* ULTRASONIC SENSOR | Resetting the triggerpin */
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    
    /* VEHICLE FUNCTION | Stopping the vehicle for 0.5 seconds after it is done turning */
    StopVehicleObsticle();
    delay(500);

    /* VEHICLE FUNCTION | Vehicle should move forwards and continuing following the line follower sensors */
    MoveVehicleForward(maxSpeed);
  }
  /* STATEMENT | The vehicle should stop completely and go into power savings mode */
  else
  {
    /* POWER SAVINGS MODE | The system will go into idle mode to reduce power usage */
    for (int i = 0; i < sleepMax; i++)
    {
      PowerSavingsMode();
    }
    /* VARIABLE | Resetting the counter after power savings mode is completed */
    blackLineCounter = 0;

    /* VEHICLE FUNCTION | Vehicle should turn 180 degrees and continuing the watering operations */
    RotateVehicleRightDegrees();
    
    /* DELAY | Using a delay to turn the vehicle */
    delay(2500);
  }
}

/* GEAR MOTOR FUNCTION | Vehicle is going forward */
void MoveVehicleForward(int maxSpeed)
{
  analogWrite(LM, maxSpeed);
  analogWrite(RM, maxSpeed);
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

/* GEAR MOTOR FUNCTION | Vehicle should turn left */
void TurnVehicleLeft()
{
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

/* GEAR MOTOR FUNCTION | Vehicle should turn left by making the left gear motor go in reverse */
void TurnVehicleLeftReverse()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, HIGH);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, LOW);
}

/* GEAR MOTOR FUNCTION | Vehicle should turn right */
void TurnVehicleRight()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

/* GEAR MOTOR FUNCTION | Vehicle should turn right by making the right gear motor go in reverse */
void TurnVehicleRightReverse()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, HIGH);
}

/* GEAR MOTOR FUNCTION | One wheels should go forward and the other one should go backwards to turn right */
void RotateVehicleRightDegrees()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, HIGH);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
}

/* GEAR MOTOR FUNCTION | One wheels should go forward and the other one should go backwards to turn left */
void RotateVehicleLeftDegrees()
{
  digitalWrite(LM_IN1, HIGH);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, LOW);
  digitalWrite(RM_IN4, HIGH);
}

/* POWER SAVING FUNCTION | The Arduino is put to sleep for 8 seconds */
void PowerSavingsMode()
{
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}

/* SOIL MOISTURE SENSOR FUNCTION | Readings from the soil moisture sensor */
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

/* SERVO MOTOR | Rotating the crane down */
void ServoMotorRotateDown()
{
  servoCraneOperation.writeMicroseconds (700);
}

/* SERVO MOTOR | Rotating the crane up */
void ServoMotorRotateUp()
{
  servoCraneOperation.writeMicroseconds (1400);
}

/* WATER PUMP FUNCTION | Activating the water pump for a few seconds */
void WaterPumpActivation()
{
  digitalWrite(waterPump, LOW);
  delay(3000);
  digitalWrite(waterPump, HIGH);
  delay(1000);
}
