#include <AccelStepper.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
int onoff;  //decides motor status
int spd;  //motorspeed in RPM
int steps; //steps taken
boolean newData = false;
char CancelText;
boolean CancelOperation = false;
//boolean motorRunning = false;


//======================


#define motorPin1 8
#define motorPin2 9
#define motorPin3 10
#define motorPin4 11
AccelStepper stepper1(4, motorPin1, motorPin3, motorPin2, motorPin4);

//===============================

//int intervalCoun; //How many intervals arduino should make
int expT; //Interval Auslöser
int nrOfPics; //Number of Photos you want to take
unsigned long myTime = 0;

//===============

void setup() {
  pinMode(7, OUTPUT); //Definition pin Auslöser
  pinMode(2, INPUT); //Definition pin Button
  Serial.begin(9600);
  Serial.println("<Arduino is ready v0.00>");
  Serial.println("Enter data in this style <ONOFF, spd, steps, expT, nrOfPics> <1, 200, 200, 1000, 4>  ");
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
        if (CancelOperation == true) {
          break;
        }
        Serial.println("Motor Start");
        FernausloeserBetaetigen();
        delay(250);
        if (CancelOperation == true) {
          break;
        }
        MotorRun();
        delay(250);
      }

    }
    else {
      Serial.println ("FALSE INPUT");
    }
    stepper1.disableOutputs();
    CancelOperation = false;
    newData = false;
    Serial.println("Done");
  }
}


//================

void MotorRun() {
  stepper1.enableOutputs();
  stepper1.setMaxSpeed(spd);
  stepper1.setAcceleration(50.0);
  stepper1.moveTo(steps);
  stepper1.run();
  while (stepper1.run() == true) {
    if (Serial.available() > 0) {
      CancelText = Serial.read();
      if(CancelText == 'a') {
        Serial.println("Cancel Motor");
        CancelOperation = true;
        stepper1.stop();
        break;
      }
    }
  }
  stepper1.setCurrentPosition(0);
  stepper1.disableOutputs();
  Serial.println("Motor is not running");
}

//====================

void FernausloeserBetaetigen() {
  myTime = millis();
  Serial.println("Photo Start");
  while (millis() < myTime + expT) {
    digitalWrite(7, HIGH);    
    if (Serial.available() > 0) {
      CancelText = Serial.read();
      if(CancelText == 'a') {
        Serial.println("Cancel Photo");
        CancelOperation = true;   
        digitalWrite(7, LOW);     
        break;
      }
    }
  }
  digitalWrite(7, LOW);
  Serial.println("Photo Ende");
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
  Serial.print("SPEED");
  Serial.println(spd);
  Serial.print("STEPS");
  Serial.println(steps);
  Serial.print("ExposureTime ");
  Serial.println(expT);
  Serial.print("Number of Photos");
  Serial.println(nrOfPics);
}
