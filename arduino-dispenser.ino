
// ------------------------------------------------------------------------------------------------------
// Resources:
// - Adjusted Example 4 from: https://forum.arduino.cc/index.php?topic=396450.msg2727728#msg2727728
// - Extend by the Arduino IR Breakbeam example: https://learn.adafruit.com/ir-breakbeam-sensors/arduino
// ------------------------------------------------------------------------------------------------------



// =============================================
// IR PART
// =============================================
// IR Breakbeam Sensor Definitions
#define LEDPIN 13  // can be removed later
#define SENSORPIN 4
 
// IR breakbeam sensor state variables
int currentSensorState = 0, lastSensorState = 0; // either LOW or HIGH (i.e., 0 or 1. NB: LOW means beam is broken, HIGH means unbroken)

// IR counter
int counter = 0;
bool isCounting = false;


// =============================================
// SERIAL INPUT PART
// =============================================
char receivedChars[32];
bool newData = false;
int dataNumber = 0; 

 
void setup() {    
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH);
  
  Serial.begin(9600);
  Serial.println("Arduino Ready");
}
 
void loop(){

  // check & receive new user input, newline should be used as EOL (LF)
  if (Serial.available() > 0) {
      // get serial input
      recvWithEndMarker();
      // cast input to int
      dataNumber = atoi(receivedChars);

      // enable counting mode
      isCounting = true;

      newData = false;
  }
  


  while (isCounting) {

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
      Serial.println("Done Counting");
      isCounting = false;
      // reset counter to 0 to prepare for next runs
      counter = 0;
    }
  }
}



void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
 
  rc = Serial.read();

  if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= 32) {
          ndx = 32 - 1;
      }
  }
  else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
  }
}
