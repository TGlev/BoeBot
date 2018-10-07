#include <Servo.h>

Servo LeftMotor;
Servo RightMotor;

Servo Arm;

const int leftCount = 1000;
const int rightCount = 2000;
const int stopCount = 1500;

const int trigPin = 8;
const int echoPin = 9;

bool isDriving = false;

void setup() {
  pinMode(A0, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  LeftMotor.attach(10);
  RightMotor.attach(11);
  Arm.attach(12);
  
  Serial.begin(9600);

  Arm.write(50);
}

void loop() 
{
  if(shouldStop())
  {
    stopMoving();
    moveArm();
    delay(700);
    moveArmBack();
  }
  else
  {
    moveForward();
  }
  
  delay(50);
}

bool shouldStop()
{
  int readVal = getDistance();
  return readVal < 6;
}

int getDistance()
{
  long duration;
  int distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance= duration*0.034/2;

  Serial.println(distance);
  return distance;
}

void moveForward()
{
  if(isDriving)
  {
    return;
  }

  isDriving = true;
  moveLeftMotor(true);
  moveRightMotor(true);
}

void stopMoving()
{
  if(!isDriving)
  {
    return;
  }

  isDriving = false;
  moveLeftMotor(false);
  moveRightMotor(false);
}

//True: Move forward; False: Stop forward;
void moveLeftMotor(bool startOrStop)
{
  if(startOrStop)
  {
    LeftMotor.write(leftCount);
  }
  else
  {
    LeftMotor.write(stopCount);
  }
}

//True: Move forward; False: Stop forward;
void moveRightMotor(bool startOrStop)
{
  if(startOrStop)
  {
    RightMotor.write(rightCount);
  }
  else
  {
    RightMotor.write(stopCount);
  }
}

void moveArm()
{
  Arm.write(500);
}

void moveArmBack()
{
  Arm.write(50);
}
