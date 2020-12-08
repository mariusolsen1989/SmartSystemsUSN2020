#include <Servo.h>
Servo servoCraneOperation;

void setup(){
  servoCraneOperation.attach(9);
}

void loop()
{
  ServoMotorRotateDown();
  delay(1000);
  
  ServoMotorRotateUp();
  delay(1000);

}

/* CRANE MOVES DOWN */
void ServoMotorRotateDown()
{
  servoCraneOperation.writeMicroseconds (900);
}

/* CRANE MOVES UP AGAIN */
void ServoMotorRotateUp()
{
  servoCraneOperation.writeMicroseconds (1800);
}
