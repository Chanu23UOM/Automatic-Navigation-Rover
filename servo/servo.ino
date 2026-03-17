#include <Servo.h>

Servo myServo;  // Create servo object
int servoPin = 9;   // Data pin connected to ~9

void setup() {
  myServo.attach(servoPin); // Attach the servo to pin 9
}

void loop() {
  // Sweep forward from 0 to 180 degrees
  for (int pos = 0; pos <= 180; pos += 1) { 
    myServo.write(pos);      // Tell servo to go to position in variable 'pos'
    delay(15);               // Wait 15ms for the servo to reach the position
  }
  
  // Sweep backward from 180 to 0 degrees
  for (int pos = 180; pos >= 0; pos -= 1) { 
    myServo.write(pos);      // Tell servo to go to position in variable 'pos'
    delay(15);               // Wait 15ms for the servo to reach the position
  }
}