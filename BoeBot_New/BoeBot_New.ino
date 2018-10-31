#include <Servo.h>

//MOTOR CONTROLS

Servo LeftMotor;
Servo RightMotor;

const int leftForwardPulse = 1520;
const int rightForwardPulse = 1480;
const int stopPulse = 1500;

int leftPulse = 1500;
int rightPulse = 1500;

//END MOTOR CONTROLS

const int foreLoopSize = 5000;
short leftForeValue = 0;
short rightForeValue = 0;

const int backLoopSize = 100;
int midValue = 0;
int leftBackValue = 0;
int rightBackValue = 0;

int foreLoopCounter = 0;
int backLoopCounter = 0;

char myStatus = 'I';

long lastStateChange;
const int jitterDelay = 200;

bool intersectionAhead;
bool intersectionComing;
long intersectionAheadTimeSet = 0;
int intersectionDelay = 3000;

void setup() {
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  
  LeftMotor.attach(11);
  RightMotor.attach(10);
  
  Serial.begin(9600);
  printWelcome();

  LeftMotor.write(stopPulse);
  RightMotor.write(stopPulse);
}
void loop() {

  bool sensor1Black = !(analogRead(A1) < 600); //rechtsvoor
  bool sensor2Black = !(analogRead(A2) < 600); //linksvoor
  bool sensor3Black = (analogRead(A3) < 30);
  bool sensor4Black = (analogRead(A4) < 30);
  bool sensor5Black = (analogRead(A5) < 30);

  if((leftForeValue > 100 || rightForeValue > 100) && !intersectionAhead && !intersectionComing)
  {
    intersectionComing = true;
  }
  if(leftForeValue <= 0 && rightForeValue <= 0 && !intersectionAhead && intersectionComing)
  {
    intersectionAhead = true;
    intersectionComing = false;
    intersectionAheadTimeSet = millis();
  }
  if((millis() - intersectionAheadTimeSet) > intersectionDelay && intersectionAhead && !intersectionComing)
  {
    intersectionAhead = false;
  }
  
  switch (myStatus)
  {
    case 'I':
      if(!sensor3Black && sensor4Black && sensor5Black)
      {
        goForward();
        changeStatus('F');
      }
    break;

    case 'F':
      if(!sensor4Black && !sensor3Black && !intersectionAhead)
      {
        //Naar rechts.
        turnRightOnAxis();
        changeStatus('R');
      }
      if(!sensor5Black && !sensor3Black && !intersectionAhead)
      {
        //Naar links
        turnLeftOnAxis();
        changeStatus('L');
      }
      if(intersectionAhead)
      {
        goForward();
      }
    break;

    case 'L':
      if(!sensor3Black && sensor5Black && sensor4Black)
      {
        goForward();
        changeStatus('F');
      }
    break;

    case 'R':
      if(!sensor3Black && sensor4Black && sensor5Black)
      {
        goForward();
        changeStatus('F');
      }
    break;
  }

  //------------------------------------

  if(sensor1Black && !(rightForeValue >= foreLoopSize))
    rightForeValue++;
  else if(!sensor1Black && (rightForeValue >= 0))
      rightForeValue--;

 if(sensor2Black && !(leftForeValue >= foreLoopSize))
    leftForeValue++;
  else if(!sensor2Black && (leftForeValue >= 0))
      leftForeValue--;

  if(sensor3Black && !(midValue >= backLoopSize))
    midValue++;
  else if(!sensor3Black && (midValue >= 0))
      midValue--;

  if(sensor4Black && !(rightBackValue >= backLoopSize))
    rightBackValue++;
  else if(!sensor4Black && (rightBackValue >= 0))
      rightBackValue--;

  if(sensor5Black && !(leftBackValue >= backLoopSize))
    leftBackValue++;
  else if(!sensor5Black && (leftBackValue >= 0))
      leftBackValue--;

  if(foreLoopCounter >= foreLoopSize)
    foreLoopCounter = 0;

  if(backLoopCounter >= backLoopSize)
    backLoopCounter = 0;
     
  foreLoopCounter++;
  backLoopCounter++;
}

void changeStatus(char newStatus)
{
  myStatus = newStatus;
  lastStateChange = millis();
}

void goForward()
{
  leftPulse = leftForwardPulse;
  rightPulse = rightForwardPulse;
  setMoveSpeeds();
}

void stopMoving()
{
  leftPulse = stopPulse;
  rightPulse = stopPulse;
  setMoveSpeeds();
}

void goRight(int power)
{
  rightPulse = rightForwardPulse + power;
  leftPulse = leftForwardPulse;
  setMoveSpeeds();
}

void goLeft(int power)
{
  rightPulse = rightForwardPulse;
  leftPulse = leftForwardPulse - power;
  setMoveSpeeds();
}

void turnRightOnAxis()
{
  rightPulse = 1520;
  leftPulse = 1520;
  setMoveSpeeds();
}

void turnLeftOnAxis()
{
  rightPulse = 1480;
  leftPulse = 1480;
  setMoveSpeeds();
}

void setMoveSpeeds()
{
  LeftMotor.write(leftPulse);
  RightMotor.write(rightPulse);
}

int dif(int a, int b)
{
  return abs(a - b);
}

void printWelcome()
{
  Serial.println("=========================");
  Serial.println("Let's roll!");
  Serial.println("=========================");
}
