const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];
int onoff;  //decides motor status
int spd;  //motorspeed in RPM
int steps; //steps taken
boolean newData = false;

//======================

const long interval = 1000; //interval at which to pause
unsigned long prevTime = 0;
boolean ledOn = false;

//======================

void setup() {
  pinMode(7, OUTPUT); 
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
    if (onoff == 1){
        TurnLedOn();
    }
    if (onoff == 0){
        TurnLedOff();
    }
    newData = false;
  }
}

//=================

void TurnLedOn() {
    digitalWrite(13,HIGH);
    Serial.println("LED ON");
    boolean ledOn = true;
}

//=================

void TurnLedOff() {
    digitalWrite(13, LOW);
    Serial.println("LED OFF");
    boolean ledOn = false;
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

}

//=======================

void showParsedData() {
  Serial.print("Message ");
  Serial.println(onoff);
  Serial.print("STEPS ");
  Serial.println(spd);
  Serial.print("Speed ");
  Serial.println(steps);
}
