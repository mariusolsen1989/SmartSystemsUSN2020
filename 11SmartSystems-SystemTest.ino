/* LIBRARIES */
#include <Servo.h>
//#include "LowPower.h"

/* VARIABLES NEEDED FOR THE POWER SAVINGS FUNCTION */
int powerSavingHours = 12;
int sleepCounter = 0;
int sleepMax = 450 * powerSavingHours;

/* SERVO MOTOR OBJECT  */
Servo servoCraneOperation;

/* WATER PUMP INPUT AND BOOLEAN FLAG */
int waterPump = 22;
bool pumpActivated = true;

/* EACH ENTRY IN THE ARRAY REPRESENTS ONE PLANT */
bool hasBeenWatered = false;

/* CHECKS IF THE PLANT IS CURRENTLY BEING WATERED */
bool isBeingWatered;

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
int maxSpeed = 60;

/* IR-SENSOR READINGS. LS = LEFT SIDE | RS = RIGHT SIDE */
int LS_PIN = A0;
int RS_PIN = A1;
int LS_STATE;
int RS_STATE;

/* COUNTER TO SEE HOW MANY TIMES DOUBLE BLACK LINES HAVE ACCURED */
int blackLineCounter = 0;

void setup() 
{
  /* SERVO ATTACHED TO PIN NUMBER 9 */
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

  Serial.begin(9600);
}

void loop() {

/* UPDATING LEFT AND RIGHT SENSOR STATES CONTINOUSLY */
LS_STATE = digitalRead(LS_PIN);
RS_STATE = digitalRead(RS_PIN);

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
distance= duration*0.034/2;

/* PRINTS THE DISTANCE ON THE MONITOR - JUST FOR TESTING PURPOSES */
Serial.print("Distance: ");
Serial.println(distance);

/* FOR PLANT NUMBER 1 */
if ((!hasBeenWatered) && (distance <= 30))
{
  delay(100);
  isBeingWatered = true;
  
  while (isBeingWatered)
  {
    StopVehicleObsticle();

    // VEHICLE SHOULD TURN RIGHT BY 90 DEGREES HERE!
    
    ServoMotorRotateDown();
    for (int i = 0; i < 10; i++)
    {
      SoilMoistureSensorReadings();
      
      /* PUMP ACTIVATION */
      if ((soilSensorValue < 900) && (isBeingWatered))
      {
        pumpActivated = true;
        
        WaterPumpActivation();
            
        pumpActivated = false;
        
        /* CHANGING 'isBeingWatered' VARIABLE TO FALSE TO EXIT THE WHILE LOOP */
        isBeingWatered = false;
        
        ServoMotorRotateUp();
        
        /* SET THE VARIABLE TO TRUE SO THAT THE SYSTEM WILL KNOW THAT THIS PLANT HAS BEEN GIVEN WATER RECENTLY. */
        hasBeenWatered = true;

        // VEHICLE SHOULD TURN LEFT BY 90 DEGREES HERE!
       }
     }
   }
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
  StopVehicleBlackLines();
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
void StopVehicleBlackLines()
{
  blackLineCounter++;
  StopVehicleObsticle();
  
  if (blackLineCounter < 2)
  {
    // THIS CODE HAVE TO BE COMPLETED DURING THE TESTING PHASE!
    // ----- The vehicle should turn left or right by 180 degrees ------
    /* THE VEHICLE WILL MOVE FORWARD AFTER TURNING */
    //MoveVehicleForward(int maxSpeed);
  }
  else
  {
    // The system should go into idle mode, reducing power usage etc.
    while (sleepCounter < sleepMax)
    {
      PowerSavingsMode();
      sleepCounter++;
    }
  }
    sleepCounter = 0;
    blackLineCounter = 0;
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

/* FUNCTION TO MAKE THE VEHICLE MOVE TO THE RIGHT */
void TurnVehicleRight()
{
  digitalWrite(LM_IN1, LOW);
  digitalWrite(LM_IN2, LOW);
  digitalWrite(RM_IN3, HIGH);
  digitalWrite(RM_IN4, LOW);
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
  delay(5000);
  digitalWrite(waterPump, HIGH);
  delay(1000);
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

/* PUT THE ARDUINO TO SLEEP FOR 8 SECONDS */
void PowerSavingsMode()
{
//  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
}










/* CODE NOT USED ANYMORE */

/* CHECKS IF THE PLANT HAS BEEN WATERED BEFORE. THIS IS FOR 1 PLANT ONLY */
// bool hasBeenWatered = false;
