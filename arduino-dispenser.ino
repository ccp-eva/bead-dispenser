
// Adjusted Example 5 from: https://forum.arduino.cc/index.php?topic=396450.0
// ... to test and debug different stepper motor steps, directions, style using Serial Monitor input

#include <Adafruit_MotorShield.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Connect a stepper motor with 200 steps per revolution (1.8 degree)
// to motor port #1 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 1);



const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

// variables to hold the parsed data
uint16_t motorSpeed = 10; // Set the delay for the Stepper Motor speed in RPM.
uint16_t motorSteps = 20; // The number of steps we want to move
uint8_t motorDir = FORWARD; // FORWARD (1), BACKWARD (2)
uint8_t motorStyle = INTERLEAVE; // How to perform each step, can be SINGLE (1), DOUBLE (2), INTERLEAVE (3) or MICROSTEP (4)


boolean newData = false;

//============

void setup() {
  Serial.begin(9600);
  Serial.println("This demo expects 4 pieces of data; motor speed, motor steps, motor direction, motor style");
  Serial.println("Motor Direction: 1 (FORWARD) or 2 (BACKWARD) - Style: 1 (SINGLE), 2 (DOUBLE), 3 (INTERLEAVE), 4 (MICROSTEP)");
  Serial.println("Enter data in this style <10, 20, 1, 1>");
  Serial.println();

  AFMS.begin();  // create with the default frequency 1.6KHz

}

//============

void loop() {
  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
  }
}

//============

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

//============

void parseData() {      // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(tempChars, ",");     // get the first part - the string
  motorSpeed = atoi(strtokIndx);  // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  motorSteps = atoi(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  motorDir = atoi(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ",");
  motorStyle = atoi(strtokIndx);     // convert this part to a float

  // EXECUTE PARSED DATA
  myMotor->setSpeed(motorSpeed);
  myMotor->step(motorSteps, motorDir, motorStyle);

  // EXECUTE SHAKING (FORWARD/BACKWORD)
  
}

//============

void showParsedData() {
  Serial.print("Motor speed (RPM): ");
  Serial.println(motorSpeed);
  Serial.print("Motor Steps: ");
  Serial.println(motorSteps);
  Serial.print("Motor Direction: ");
  Serial.println(motorDir);
  Serial.print("Motor Style: ");
  Serial.println(motorStyle);
}
