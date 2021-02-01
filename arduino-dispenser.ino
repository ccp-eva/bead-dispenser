
// ------------------------------------------------------------------------------------------------------
// Resources:
// - Adjusted Example 1 from: https://forum.arduino.cc/index.php?topic=396450.0
//   - Use "No line ending" in Serial Monitor (to avoid double prints)
// - Extend by the Arduino IR Breakbeam example: https://learn.adafruit.com/ir-breakbeam-sensors/arduino
// ------------------------------------------------------------------------------------------------------




// Adafruit includes (todo: is Wire.h needed? Or just for non-standard calls such as I2C: Adafruit_MotorShield(0x61))
#include <Wire.h>
#include <Adafruit_MotorShield.h>


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


// =============================================
// SERIAL INPUT PART
// =============================================
char receivedChar;
boolean newData = false;

 
void setup() {
  // initialize the LED pin as an output: // can be removed later
  pinMode(LEDPIN, OUTPUT);      
  // initialize the sensor pin as an input:
  pinMode(SENSORPIN, INPUT);     
  digitalWrite(SENSORPIN, HIGH); // turn on the pullup
  
  Serial.begin(9600);
  Serial.println("Arduino Ready ");
}
 
void loop(){
  // read the state of the pushbutton value:
  currentSensorState = digitalRead(SENSORPIN);



  // LED part
  // ------ CAN BE REMOVED LATER ------
  // check if the sensor beam is broken
  // if it is, the currentSensorState is LOW:
  if (currentSensorState == LOW) {     
    // turn LED on to indicate broken beam:
    digitalWrite(LEDPIN, HIGH);  
  } 
  else {
    // turn LED off to indicate unbroken beam:
    digitalWrite(LEDPIN, LOW); 
  }
  // ------ CAN BE REMOVED LATER ------




  // Get new serial input (after pressing enter serial.available is fired)
  if (Serial.available() > 0) {
      receivedChar = Serial.read();
      newData = true;
  }

  
  // If there is a new input process it
  if (newData == true) {
      Serial.print("Counting to: ");
      Serial.println(receivedChar);
      newData = false;
  }



  // if there is a diff in currentSensorState and lastSensorState
  if (currentSensorState == HIGH && lastSensorState == LOW) {
    Serial.println("Unbroken");
  } 
  if (currentSensorState == LOW && lastSensorState == HIGH) {
    Serial.println("Broken");
    
    // if beam is broken we can assume that an object is blocking it, so incrementing the counter
    counter += 1;
  }

  // Serial.println(counter);
  // synchronize states (this should be at the end)
  lastSensorState = currentSensorState;
}
