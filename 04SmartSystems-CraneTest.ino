/* NEED TO INCLUDE THE SERVO LIBRARY */
#include <Servo.h>

/* CREATES AN OBJECT TO BE ABLE TO CONTROL THE SERVO MOTOR */
Servo servoCraneOperation;

/* DEFINES A VARIABLE TO BE ABLE TO STORE THE POSITION OF THE SERVO MOTOR */
int servoPosition = 0;

/* DEFINES WATER PUMP INPUTS */
int waterPump = 24;

/* CHECKS IF THE PLANT HAS BEEN WATERED BEFORE. THIS IS FOR 1 PLANT ONLY */
bool hasBeenWatered = false;

/* CHECKS IF THE PLANT IS CURRENTLY BEING WATERED */
bool isBeingWatered;

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
  digitalWrite(waterPump, LOW);

  Serial.begin(9600);
}

void loop() 
{
    /* IF THE PLANT HASN'T BEEN WATERED BEFORE AND THE DISTANCE VARIABLE FROM THE ULTRASONIC SENSOR EVALUATES TO TRUE - THE VEHICLE SHOULD STOP. */
  if (!hasBeenWatered)
  {
    /* ADDING A SMALL DELAY BEFORE STOPPING THE VEHICLE. */
    delay(200);

    /* CHANGING 'isBeingWatered' VARIABLE TO TRUE TO ENTER THE WHILE LOOP */
    isBeingWatered = true;
    while (isBeingWatered)
    {
      /* CODE FOR SERVO MOTOR (GOING DOWN) SHOULD BE ADDED HERE. */ 
      ServoMotorRotateDown();
      /* CODE FOR SOIL MOISTURE SENSOR SHOULD BE ADDED HERE. */
      soilMoistureSensorReadings();
      /* CODE FOR PUMP ACTIVATION SHOULD BE ADDED HERE */
      if (sensorValue < 500)
      {
        /* ADDING A 2 SECOND DELAY HERE */
        delay(2000);
        WaterPumpActivation();
      }
      /* CODE FOR SERVO MOTOR (GOING UP AGAIN) SHOULD BE ADDED HERE. */
      void ServoMotorRotateUp(); 
      /* CHANGING 'isBeingWatered' VARIABLE TO FALSE TO EXIT THE WHILE LOOP */
      isBeingWatered = false;
    }
    /* SET THE VARIABLE TO TRUE SO THAT THE SYSTEM WILL KNOW THAT THIS PLANT HAS BEEN GIVEN WATER RECENTLY. */
    hasBeenWatered = true;
  }
}

/* FUNCTION THAT WILL MOVE THE CRANE DOWN */
/* THIS HAVE NOT BEEN TESTED YET */
void ServoMotorRotateDown()
{
  /* MAKES THE SERVO OPERATE 45 DEGREES */
  for (servoPosition = 0; servoPosition <= 70; servoPosition += 1) 
   {
      /* TELLS THE SERVO TO GO TO POSITION STORED IN THE VARIABLE servoPosition */
      servoCraneOperation.write(servoPosition);
      /* WAITING 100 MS FOR THE SERVO TO MOVE TO THE POSITION. */
      delay(20);
   }
}

/* FUNCTION THAT WILL MOVE THE CRANE BACK UP AGAIN */
/* THIS HAVE NOT BEEN TESTED YET */
void ServoMotorRotateUp()
{
    /* MAKES THE SERVO OPERATE 45 DEGREES IN THE OPPOSIT DIRECTION */
   for (servoPosition = 70; servoPosition >= 0; servoPosition -= 1) 
   {
      /* TELLS THE SERVO TO GO TO POSITION STORED IN THE VARIABLE servoPosition */
      servoCraneOperation.write(servoPosition);
      /* WAITING 100 MS FOR THE SERVO TO MOVE TO THE POSITION. */
      delay(20);
    }
}

/* FUNCTION FOR ACTIVATING THE WATER PUMP. THIS WILL BE ACTIVATED FOR 1 SECOND AND THEN TURNED OFF FOR 1 SECOND */
void WaterPumpActivation()
{
  digitalWrite(waterPump, LOW);
  delay(1000);
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
  delay(20);
}
