// ------------------------------------------------------------------------------------------------------
// Resources:
// - Adjusted Example 4 from: https://forum.arduino.cc/index.php?topic=396450.msg2727728#msg2727728
//   - make sure to set EOL to Newline (LF)
// - Extend by the Arduino IR Breakbeam example: https://learn.adafruit.com/ir-breakbeam-sensors/arduino
// ------------------------------------------------------------------------------------------------------

#include <Wire.h>
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Connect a stepper motor with 200 steps per revolution (1.8 degree) to motor port #1 (M1 and M2)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 1);

// =============================================
// IR BREAKBEAM PART
// =============================================
// IR Breakbeam Sensor Definitions
#define SENSORPIN 4
// IR breakbeam sensor state variables
int currentSensorState = 0, lastSensorState = 0; // either LOW or HIGH (i.e., 0 or 1. NB: LOW means beam is broken, HIGH means unbroken)

// IR counter
int counter = 0;
bool isCounting = false;

// =============================================
// SERIAL INPUT PART
// =============================================
char receivedChars[32]; // an array to store the received data
bool newData = false;
int dataNumber = 0;

// =============================================
// MOTOR
// =============================================
// SINGLE may create bad motion behavior!
void forwardstep1() {  
  myMotor->onestep(FORWARD, DOUBLE);
}
void backwardstep1() {  
  myMotor->onestep(BACKWARD, DOUBLE);
}

AccelStepper Astepper1(forwardstep1, backwardstep1); // use functions to step


// =============================================
// SETUP
// =============================================
void setup() {
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT_PULLUP);

  AFMS.begin();
  Astepper1.setMaxSpeed(500);
  
  Serial.begin(9600);
  Serial.println("Arduino Ready");
}

// =============================================
// LOOP
// =============================================
void loop() {

  // get serial input
  recvWithEndMarker();

  // initialize motor with given user input
  if (newData == true) {
    initMotor();
  }
}

// =============================================
// FUNCTIONS
// =============================================

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  if (Serial.available() > 0) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= 32) {
        ndx = 32 - 1;
      }
    } else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void initMotor() {
  dataNumber = 0;
  // cast char to int
  dataNumber = atoi(receivedChars);
  Serial.print("Counting to ... ");
  Serial.println(dataNumber);

  // start counting mode
  isCounting = true;

  while (isCounting) {
    // start Motor
    Astepper1.setSpeed(300);
    Astepper1.runSpeed();
    // --------------------------------------------------------------------------------------------------
    // Sensor Algorithm
    // --------------------------------------------------------------------------------------------------
    // read the current sensor state
    currentSensorState = digitalRead(SENSORPIN);
    // if there is a diff in currentSensorState and lastSensorState
    if (currentSensorState == HIGH && lastSensorState == LOW) {
      Serial.println("Unbroken");
    }
    if (currentSensorState == LOW && lastSensorState == HIGH) {
      Serial.println("Broken");

      // if beam is broken we can assume that an object is blocking it, so incrementing the counter
      counter += 1;
    }
    // --------------------------------------------------------------------------------------------------

    Serial.println(counter); 


    // synchronize states (this should be at the end)
    lastSensorState = currentSensorState;

    // reset isCounting when reached the counter
    if (dataNumber == counter) {
      // stop motor
      Astepper1.stop();
      Serial.println("Done Counting");
      isCounting = false;
      // reset counter to 0 to prepare for next runs
      counter = 0;
    }
  }

  // reset new data to avoid re-initing the motor
  newData = false;
}
