#include <Stepper.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
int onoff;  //decides motor status
int spd;  //motorspeed in RPM
int steps; //steps taken
boolean newData = false;
//boolean motorRunning = false;


//======================


#define motorPin1 8
#define motorPin2 9
#define motorPin3 10
#define motorPin4 11
const float STEPS_PER_REV = 32;
const float GEAR_RED = 64;
const float STEPS_PER_OUT_REV = STEPS_PER_REV * GEAR_RED;
Stepper stepper1(STEPS_PER_REV, motorPin1, motorPin3, motorPin2, motorPin4);

//===============================

//int intervalCoun; //How many intervals arduino should make
int expT; //Interval Auslöser
int nrOfPics; //Number of Photos you want to take
//unsigned long prevTime = 0;

//===============

void setup() {
  pinMode(7, OUTPUT); //Definition pin Auslöser
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  Serial.println("Enter data in this style <ONOFF, stp, spd, expT, nrOfPics> <1, 300, 400, 1000, 5>  ");
}

//=============

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseData();
    showParsedData();
    if (onoff == 1) {
      for (int i = 0; i < nrOfPics; i++)
      {
        Serial.println("Motor Start");
        unsigned long Timer = millis();
        MotorRun();
        delay(250);
        FernausloeserBetaetigen();
        delay(250);
      }

    }
    else {
      Serial.println ("FALSE INPUT");
    }
    newData = false;
  }
}

//=================

void MotorRun() {
  stepper1.setSpeed(spd);
  stepper1.step(steps);
}

//====================

void FernausloeserBetaetigen() {
  digitalWrite(7, HIGH);
  delay(expT); //Betätigungszeit
  digitalWrite(7, LOW);
}

//====================
void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}

//==============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");    // get the first part - the string
  onoff = atoi(strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  spd = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  steps = atoi(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ",");
  expT = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  nrOfPics = atoi(strtokIndx);
}

void showParsedData() {
  Serial.print("Message ");
  Serial.println(onoff);
  Serial.print("STEPS ");
  Serial.println(spd);
  Serial.print("Speed ");
  Serial.println(steps);
  Serial.print("ExposureTime ");
  Serial.println(expT);
  Serial.print("Number of Photos");
  Serial.println(nrOfPics);
}
