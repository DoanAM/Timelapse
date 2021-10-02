#include <Stepper.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
int integerFromPC_1;
int integerFromPC_2;
int integerFromPC_3;
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




const long interval = 1000; //interval at which to pause
unsigned long prevTime = 0;

//===============

void setup() {
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
  Serial.println("Enter data in this style <1, 300, 400>  ");
}

//=============

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    parseData();
    showParsedData();
    if (integerFromPC_1 == 1) {
      for (int n = 0; n <= 3; n++) {
        Serial.println("Motor Start");
        unsigned long Timer = millis();
        if (Timer - prevTime >= interval) {
          MotorRun();
          prevTime = Timer;
        }
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
  stepper1.setSpeed(integerFromPC_2);
  stepper1.step(integerFromPC_3);
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
  integerFromPC_1 = atoi(strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  integerFromPC_2 = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  integerFromPC_3 = atoi(strtokIndx);     // convert this part to a float

}

void showParsedData() {
  Serial.print("Message ");
  Serial.println(integerFromPC_1);
  Serial.print("STEPS ");
  Serial.println(integerFromPC_2);
  Serial.print("Speed ");
  Serial.println(integerFromPC_3);
}
