#include <AFMotor.h>

// Initialize all 4 motors on the L293D shield
// Assuming Left side = M1 & M2, Right side = M3 & M4
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// Define Ultrasonic Sensor Pins
const int trigPin = A1;
const int echoPin = A2;

// ⚙️ Navigation Tuning Variables
const int safeDistance = 15; // Minimum distance in cm before stopping
const int turnTime = 500;    // Milliseconds to spin 90 degrees (Adjust this during testing!)

void setup() {
  Serial.begin(9600);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set initial speed (0-255)
  int speed = 120; 
  motor1.setSpeed(speed);
  motor2.setSpeed(speed);
  motor3.setSpeed(speed);
  motor4.setSpeed(speed);

  stopMotors();
  Serial.println("🤖 Autonomous Rover Ready!");
  Serial.println("Starting in 3 seconds...");
  delay(3000); // Gives you 3 seconds to put the rover on the floor
}

void loop() {
  long distance = measureDistance();

  // If path is clear (or sensor returns 999 for out-of-range), keep driving
  if (distance > safeDistance || distance == 999) {
    moveForward();
  } else {
    // 🛑 Obstacle detected! Execute scanning sequence.
    stopMotors();
    Serial.println("⚠️ Obstacle detected! Scanning...");
    scanAndDecide();
  }
  
  delay(50); // Tiny delay to keep sensor readings stable
}

// --- Autonomous Decision Logic ---

void scanAndDecide() {
  long rightDistance, leftDistance;

  // 1. Look Right
  turnRight();
  delay(turnTime);
  stopMotors();
  delay(200); // Let the chassis settle before measuring
  rightDistance = measureDistance();
  
  // 2. Look Left (Spin double the time to pass the center point)
  turnLeft();
  delay(turnTime * 2);
  stopMotors();
  delay(200);
  leftDistance = measureDistance();
  
  // 3. Return to Center
  turnRight();
  delay(turnTime);
  stopMotors();
  delay(200);

  // 4. Compare and Execute the Best Path
  if (rightDistance > leftDistance && rightDistance > safeDistance) {
    Serial.println("➡️ Path clear on the RIGHT. Turning...");
    turnRight();
    delay(turnTime); 
  } 
  else if (leftDistance > rightDistance && leftDistance > safeDistance) {
    Serial.println("⬅️ Path clear on the LEFT. Turning...");
    turnLeft();
    delay(turnTime);
  } 
  else {
    // Corner Trap: Both sides are blocked!
    Serial.println("🔄 Trapped! Reversing and doing a U-Turn.");
    moveBackward();
    delay(500);
    turnRight();
    delay(turnTime * 2); // Spin ~180 degrees
  }
  
  stopMotors(); // Brief pause before resuming main loop
}

// --- Sensor & Motor Functions ---

long measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 30ms timeout prevents Arduino from freezing if echo is lost
  long duration = pulseIn(echoPin, HIGH, 30000);
  
  if (duration == 0) return 999; 
  return (duration / 2) / 29.1;
}

void moveForward() {
  // 🔄 INVERTED: Uses BACKWARD to move physically forward
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void moveBackward() {
  // 🔄 INVERTED: Uses FORWARD to move physically backward
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void turnLeft() {
  // Smooth Left Turn: Left wheels reverse, Right wheels forward
  motor1.run(FORWARD);  // Physical Reverse
  motor2.run(FORWARD);  // Physical Reverse
  motor3.run(BACKWARD); // Physical Forward
  motor4.run(BACKWARD); // Physical Forward
}

void turnRight() {
  // Smooth Right Turn: Left wheels forward, Right wheels reverse
  motor1.run(BACKWARD); // Physical Forward
  motor2.run(BACKWARD); // Physical Forward
  motor3.run(FORWARD);  // Physical Reverse
  motor4.run(FORWARD);  // Physical Reverse
}

void stopMotors() {
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}