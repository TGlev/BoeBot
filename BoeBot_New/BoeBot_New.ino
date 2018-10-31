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
  //Alle pins instellen als input pins
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  //Verbinding maken met de motoren
  LeftMotor.attach(11);
  RightMotor.attach(10);

  //Seriële communicatie starten en het welkom bericht printen
  Serial.begin(9600);
  printWelcome();

  //Allebei de motoren hun stopPulse geven zodat ze stil staan.
  LeftMotor.write(stopPulse);
  RightMotor.write(stopPulse);
}
void loop() {

  //Alle waarden van de vijf sensoren inlezen.
  bool sensor1Black = (analogRead(A1) < 400); //rechtsvoor
  bool sensor2Black = (analogRead(A2) < 400); //linksvoor
  bool sensor3Black = (analogRead(A3) < 400); //midden
  bool sensor4Black = (analogRead(A4) < 400); //linksachter
  bool sensor5Black = (analogRead(A5) < 400); //rechtsachter

  /*
   * We werken met een status systeem.
   * De robot bevind zich in een bepaalde status waar in hij dingen doet. Deze status is aangegeven door de myStatus char.
   * 
   * De robot start in status I (init). Zodra hij in het midden een lijn detecteert, gaat hij vooruit rijden en zet hij zijn status naar F (forward)
   * In F kijkt hij of hij van de lijn afwijkt, dit doet hij door naar sensor 3 en 4 te kijken. Zodra sensor4 niet meer zwart detecteert, gaat hij naar rechts draaien en in de status R (Right)
   * Hetzelfde geldt voor sensor 3, dan gaat hij naar links.
   * 
   * De intersectionAhead boolean is bedoeld om zijn sensoren te negeren als hij op een kruising afrijd. Zie hieronder voor meer details.
   */
  switch (myStatus)
  {
    case 'I':
      //Kijken of we een lijn detecteren.
      if(!sensor3Black && sensor4Black && sensor5Black)
      {
        //Naar voren rijden
        goForward();
        
        //Status veranderen
        changeStatus('F');
      }
    break;

    case 'F':
      if(!sensor4Black && !sensor3Black && !intersectionAhead)
      {
        //Naar rechts om zijn eigen as
        turnRightOnAxis();
        changeStatus('R');
      }
      if(!sensor5Black && !sensor3Black && !intersectionAhead)
      {
        //Naar links om zijn eigen as
        turnLeftOnAxis();
        changeStatus('L');
      }
      if(intersectionAhead)
      {
        //Er komt een kruising aan, negeer je sensoren en rijd gewoon rechtdoor
        goForward();
      }
    break;

    case 'L':
      //Kijken of je weer boven de lijn zit
      if(!sensor3Black && sensor5Black && sensor4Black)
      {
        //Weer vooruit rijden
        goForward();
        changeStatus('F');
      }
    break;

    case 'R':
      //Kijken of je weer boven de lijn zit
      if(!sensor3Black && sensor4Black && sensor5Black)
      {
        //Weer vooruit rijden
        goForward();
        changeStatus('F');
      }
    break;
  }

  //Detecteren of er een kruising aankomt
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

  /*
   * Afvlakken van de ruwe sensordata zodat de robot niet heftig en abrupt reageert
   * Wordt NOG niet gebruikt voor het volgen van de lijn, moet ik nog doen.
   * Wordt WEL gebruikt voor het detecteren van een kruising. Zie code hierboven.
   */

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
