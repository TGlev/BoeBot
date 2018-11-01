#include <Servo.h>

const int trigPin = 3;
const int echoPin = 4;

Servo LeftMotor;
Servo RightMotor;
Servo Arm;

//Minimum speed: 20
//Maximum speed: 100
int mySpeed = 40;
float turnFactor = 1.05;

int leftForwardPulse = 1500;
int rightForwardPulse = 1500;
const int stopPulse = 1500;

int leftPulse = 1500;
int rightPulse = 1500;

char myStatus = 'I';

int whiteVal = 300;

bool leftDetected;
bool rightDetected;

bool isArmRight = false;

void setup() {
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Arm.attach(9);
  LeftMotor.attach(11);
  RightMotor.attach(10);
  
  Serial.begin(9600);
  printWelcome();

  LeftMotor.write(stopPulse);
  RightMotor.write(stopPulse);

  leftForwardPulse += mySpeed;
  rightForwardPulse -= mySpeed;
  
  Arm.write(0);
}

void loop() 
{
  /*Serial.print(analogRead(A3));
  Serial.print(", ");
  Serial.print(analogRead(A5));
  Serial.print(", ");
  Serial.print(analogRead(A4));
  Serial.print(", ");
  Serial.print(analogRead(A1));
  Serial.print(", ");
  Serial.println(analogRead(A2));*/
  //delay(1000);
  //return;

  bool MiddleOnPath = !(analogRead(A3) > whiteVal);
  bool TopLeftOnPath = !(analogRead(A5) > whiteVal);
  bool TopRightOnPath = !(analogRead(A4) > whiteVal);
  bool BotLeftOnPath = !(analogRead(A1) > whiteVal);
  bool BotRightOnPath = !(analogRead(A2) > whiteVal);

  /*Serial.print(MiddleOnPath);
  Serial.print(", ");
  Serial.print(TopLeftOnPath);
  Serial.print(", ");
  Serial.print(TopRightOnPath);
  Serial.print(", ");
  Serial.print(BotLeftOnPath);
  Serial.print(", ");
  Serial.println(BotRightOnPath);
  delay(1000);
  return;*/

  if(TopLeftOnPath)
  {
    leftDetected = true;
    rightDetected = false;
  }

  if(TopRightOnPath)
  {
    rightDetected = true;
    leftDetected = false;
  } 
  
  switch (myStatus)
  {
    //Iniatilize phase.
    //We gaan kijken of we op de lijn staan en zoja gaan we rijden. Naar F
    case 'I':

      if(MiddleOnPath && BotLeftOnPath && BotRightOnPath && !TopLeftOnPath && !TopRightOnPath)
      {
        goForward();
        changeStatus('F');
      }
      
    break;

    /*
     * We rijden vooruit
     * Als de middelste sensor de lijn ziet, rijden we op basis van de linker- en rechter-ONDER sensor
     * Als de middelste sensor de lijn niet ziet, rijden we op basis van de linker- en rechter-BOVEN sensor
     */
    case 'F':

      //leftDetected = false;
      //rightDetected = false;
      //if(MiddleOnPath)
      //{

      if(getDistance() < 10)
      {
        stopMoving();
        changeStatus('C');
      }
      
      if(BotLeftOnPath && !BotRightOnPath)
      {
        goLeft(mySpeed / turnFactor);
        changeStatus('L');
      }
      if(!BotLeftOnPath && BotRightOnPath)
      {
        goRight(mySpeed / turnFactor);
        changeStatus('R');
      }
    break;

    //We corrigeren naar links op basis van de linker-onder sensor
    //Als we de lijn weer netjes zien gaan we terug naar F
    case 'L':
      if(BotLeftOnPath && BotRightOnPath)
      {
        goForward();
        changeStatus('F');
      }

      if(!MiddleOnPath && !BotLeftOnPath && !BotRightOnPath && !TopLeftOnPath && !TopRightOnPath)
      {
        if(leftDetected && !rightDetected)
        {
          turnLeftOnAxis();
          changeStatus('B');
        }
        else if(!leftDetected && rightDetected)
        {
          turnRightOnAxis();
          changeStatus('b');
        }
        else
        {
          turnLeftOnAxis();
          changeStatus('B');
        }
      }
    break;

    //We corrigeren naar rechts op basis van de rechter-onder sensor
    //Als we de lijn weer netjes zien gaan we terug naar F
    case 'R':
      if(BotLeftOnPath && BotRightOnPath)
      {
        goForward();
        changeStatus('F');
      }

      if(!MiddleOnPath && !BotLeftOnPath && !BotRightOnPath && !TopLeftOnPath && !TopRightOnPath)
      {
        if(leftDetected && !rightDetected)
        {
          turnLeftOnAxis();
          changeStatus('B');
        }
        else if(!leftDetected && rightDetected)
        {
          turnRightOnAxis();
          changeStatus('b');
        }
        else
        {
          turnRightOnAxis();
          changeStatus('b');
        }
      }
    break;

    case 'b':

    if(MiddleOnPath)
    {
      delay(50);
      turnRightOnAxis();
      changeStatus('3');
    }
    break;

    case 'B':

    if(MiddleOnPath)
    {
      //delay(50);
      turnLeftOnAxis();
      changeStatus('9');
    }
    break;

    case '3':
      if(MiddleOnPath)
      {
        goForward();
        changeStatus('F');
      }
    break;

    case '9':
      if(MiddleOnPath)
      {
        goForward();
        changeStatus('F');
      }
    break;

    case 'C':

    
    if(getDistance() > 20)
    {
      goForward();
      changeStatus('F');
      break;
    }

    if(isArmRight)
      Arm.write(0);
    else
      Arm.write(180);
    
    break;
  }

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

void changeStatus(char newStatus)
{
  myStatus = newStatus;
  //Serial.println(newStatus);
}

void goForward()
{
  leftPulse = leftForwardPulse;
  rightPulse = rightForwardPulse;
  setMoveSpeeds();
}

void goBackwards()
{
  leftPulse = 1480;
  rightPulse = 1520;
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
  stopMoving();
  if(power > mySpeed)
    power = mySpeed;
    
  rightPulse = rightForwardPulse + power;
  leftPulse = leftForwardPulse;
  setMoveSpeeds();
}

void goLeft(int power)
{
  stopMoving();
  if(power > mySpeed)
    power = mySpeed;
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

void printWelcome()
{
  Serial.println("=========================");
  Serial.println("Let's roll!");
  Serial.println("=========================");
}
