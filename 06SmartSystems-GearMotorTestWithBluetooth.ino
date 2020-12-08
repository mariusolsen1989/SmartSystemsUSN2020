#include<SoftwareSerial.h>

SoftwareSerial bt(0,1);                                                     // Assigning 10 as RX and 11 as TX 

// Motor A (Left Motor Connections)
int motorLeft = 4;
int motorLeft_in1 = 5;
int motorLeft_in2 = 6;

// Motor B (Right Motor Connections)
int motorRight = 9;
int motorRight_in3 = 7;
int motorRight_in4 = 8;

void setup() {
  // Set all the motor control pins to outputs
  pinMode(motorLeft, OUTPUT);
  pinMode(motorRight, OUTPUT);
  pinMode(motorLeft_in1, OUTPUT);
  pinMode(motorLeft_in2, OUTPUT);
  pinMode(motorRight_in3, OUTPUT);
  pinMode(motorRight_in4, OUTPUT);
  
  // Turn off motors - Initial state
  digitalWrite(motorLeft_in1, LOW);
  digitalWrite(motorLeft_in2, LOW);
  digitalWrite(motorRight_in3, LOW);
  digitalWrite(motorRight_in4, LOW);

  while(!Serial){;}                                                           // Waiting for serial communication to setup

  bt.begin(9600);                                                             // Beginning the bluetooth connection
}

void loop() {
  
  bt.listen();                                                                // Listening to the bluetooth

  while(bt.available())                                                       // Till' we are receiving the input continue in the loop
  {
      char ch = bt.read();                                                    // Reading one character at a time

      if(ch=='F'){                                                            // Action to be performed if input is 'F'
        analogWrite(motorLeft, 150);                                           // Changed maximum speed to 60.
        analogWrite(motorRight, 250);                                          // Changed maximum speed to 60.
        digitalWrite(motorLeft_in1, LOW);
        digitalWrite(motorLeft_in2, HIGH);
        digitalWrite(motorRight_in3, LOW);
        digitalWrite(motorRight_in4, HIGH);
      }
      
      else if(ch == 'B'){                                                     // Action to be performed if input is 'B'
        analogWrite(motorLeft, 150);                                           // Changed maximum speed to 60.
        analogWrite(motorRight, 250);                                          // Changed maximum speed to 60.
        digitalWrite(motorLeft_in1, HIGH);
        digitalWrite(motorLeft_in2, LOW);
        digitalWrite(motorRight_in3, HIGH);
        digitalWrite(motorRight_in4, LOW);
      }
      
      else if(ch == 'L'){                                                     // Action to be performed if input is 'L'
        digitalWrite(motorLeft_in1, LOW);
        digitalWrite(motorLeft_in2, LOW);
        digitalWrite(motorRight_in3, LOW);
        digitalWrite(motorRight_in4, HIGH);
      }
      
      else if(ch == 'R'){                                                     // Action to be performed if input is 'R'
        digitalWrite(motorLeft_in1, LOW);
        digitalWrite(motorLeft_in2, HIGH);
        digitalWrite(motorRight_in3, LOW);
        digitalWrite(motorRight_in4, LOW);
      }
      else if(ch =='S'){                                                      // Action to be performed if input is 'S'
        digitalWrite(motorLeft_in1, LOW);
        digitalWrite(motorLeft_in2, LOW);
        digitalWrite(motorRight_in3, LOW);
        digitalWrite(motorRight_in4, LOW);
      }
  }
}
