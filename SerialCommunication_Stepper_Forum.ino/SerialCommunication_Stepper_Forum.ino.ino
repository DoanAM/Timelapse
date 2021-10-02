#include <Stepper.h>

const byte numChars = 32;
char receivedChars [numChars];
char tempChars     [numChars];

int integerFromPC_1;    // `Vorformatierter Text`
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

const unsigned long interval = 1000; //interval at which to pause
unsigned long prevTime = 0;

//===============
void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");
    Serial.println("Enter data in this style <1, 300, 400>  ");
}
//=============
void loop() {
    if (Serial.available ())  {
        int n = Serial.readBytesUntil ('>', receivedChars, numChars);
        receivedChars [n] = 0;
        sscanf (receivedChars, "<%d,%d,%d",
            & integerFromPC_1, & integerFromPC_2, & integerFromPC_3);
    }

    if (0 < integerFromPC_1) {
         unsigned long timer = millis();
         if ( (timer - prevTime) >= interval) {
             MotorRun();
             prevTime = timer;
             integerFromPC_1--;
        }
    }
}
//=================
void MotorRun() {

    stepper1.setSpeed(integerFromPC_2);
    stepper1.step(integerFromPC_3);

    char s [80];
    sprintf (s, "%s: spd %3d, steps %d",
            __func__, integerFromPC_2, integerFromPC_3);
    Serial.println (s);

}
