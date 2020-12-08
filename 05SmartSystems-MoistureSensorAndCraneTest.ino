/* NEED TO INCLUDE THE SERVO LIBRARY */
#include <Servo.h>

/* CREATES AN OBJECT TO BE ABLE TO CONTROL THE SERVO MOTOR */
Servo servoCraneOperation;

/* DEFINES WATER PUMP INPUTS */
int waterPump = 28;

bool pumpActivated = false;

/* CHECKS IF THE PLANT HAS BEEN WATERED BEFORE. THIS IS FOR 1 PLANT ONLY */
bool plantHasBeenWatered = false;

/* CHECKS IF THE PLANT IS CURRENTLY BEING WATERED */
bool plantIsBeingWatered;

/* DEFINES THE SENSOR PIN FOR THE SOIL MOISTURE SENSOR - WILL BE USING ANALOG CONNECTION 2 (A2) FOR NOW */
#define sensorPin A2

/* NEED A VARIABLE TO STORE SENSOR READINGS */
float sensorValue = 0;

void setup() 
{
  /* ATTACHES THE SERVO ON PIN NUMBER 8 TO THE SERVO OBJECT (servoCraneOperation) */
  servoCraneOperation.attach(9);

  /* WATERPUMP SET TO OUTPUT & INITIALLY SET IT TO LOW */
  pinMode(waterPump, OUTPUT);
  digitalWrite(waterPump, HIGH);

  Serial.begin(9600);
}

void loop() 
{
    /* IF THE PLANT HASN'T BEEN WATERED BEFORE AND THE DISTANCE VARIABLE FROM THE ULTRASONIC SENSOR EVALUATES TO TRUE - THE VEHICLE SHOULD STOP. */
  if (!plantHasBeenWatered)
  {
    /* ADDING A SMALL DELAY BEFORE STOPPING THE VEHICLE. */
    delay(200);

    /* CHANGING 'isBeingWatered' VARIABLE TO TRUE TO ENTER THE WHILE LOOP */
    plantIsBeingWatered = true;
    while (plantIsBeingWatered)
    {
      /* CODE FOR SERVO MOTOR (GOING DOWN) SHOULD BE ADDED HERE. */ 
      ServoMotorRotateDown();
      /* CODE FOR SOIL MOISTURE SENSOR SHOULD BE ADDED HERE. */

      for (int i = 0; i < 10; i++)
      {
        soilMoistureSensorReadings();
        /* CODE FOR PUMP ACTIVATION SHOULD BE ADDED HERE */
        if ((sensorValue < 800) && (plantIsBeingWatered))
        {
          WaterPumpActivation();
          
          /* CHANGING 'isBeingWatered' VARIABLE TO FALSE TO EXIT THE WHILE LOOP */
          plantIsBeingWatered = false;
        }
      }
    }
       delay(2000);    
       ServoMotorRotateUp();
       
       /* SET THE VARIABLE TO TRUE SO THAT THE SYSTEM WILL KNOW THAT THIS PLANT HAS BEEN GIVEN WATER RECENTLY. */
       plantHasBeenWatered = true;
  }
}

/* FUNCTION THAT WILL MOVE THE CRANE DOWN */
/* THIS HAVE NOT BEEN TESTED YET */
void ServoMotorRotateDown()
{
  servoCraneOperation.writeMicroseconds (900);
}

/* FUNCTION THAT WILL MOVE THE CRANE BACK UP AGAIN */
/* THIS HAVE NOT BEEN TESTED YET */
void ServoMotorRotateUp()
{
  servoCraneOperation.writeMicroseconds (1800);
}

/* FUNCTION FOR ACTIVATING THE WATER PUMP. THIS WILL BE ACTIVATED FOR 1 SECOND AND THEN TURNED OFF FOR 1 SECOND */
void WaterPumpActivation()
{
  digitalWrite(waterPump, LOW);
  delay(3000);
  digitalWrite(waterPump, HIGH);
  delay(1000);
}


void soilMoistureSensorReadings()
{
  for (int i = 0; i <= 100; i++)
  {
    sensorValue = sensorValue + analogRead(sensorPin);
    delay(1);
  }
  sensorValue = sensorValue / 100.0;
  Serial.println(sensorValue);
  delay(20);
}
