#include <SoftwareSerial.h>

SoftwareSerial BTSerial(2, 3); // RX, TX

const int potPin = A0;   // Connect potentiometer to analog pin A0
const int led1Pin = 4;   // Connect LED1 to digital pin 4
const int led2Pin = 5;   // Connect LED2 to digital pin 5
const int led3Pin = 6;   // Connect LED3 to digital pin 6
const int led4Pin = 7;   // Connect LED4 to digital pin 7
const int switchPin = 8; // Connect switch to digital pin 8

bool switchState = false;
unsigned long previousMillis = 0; // Variable to store the last time LED1 was updated
int duration = 0; // Variable to hold the duration in milliseconds
bool stopReading = false; // Variable to stop reading potentiometer

String msg, cmd;

void setup() {
  Serial.begin(9600);     // Initialize serial communication
  BTSerial.begin(9600);   // Initialize Bluetooth serial communication
  Serial.println("Bluetooth communication initialized");
  msg = "";
  Serial.println("Waiting for commands...");

  pinMode(led1Pin, OUTPUT); // Set LED pins as output
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  pinMode(led4Pin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); // Set switch pin as input with internal pull-up resistor

  digitalWrite(led1Pin, LOW); // Turn off LED1 initially
  digitalWrite(led2Pin, LOW); // Turn off LED2
  digitalWrite(led3Pin, LOW); // Turn off LED3
  digitalWrite(led4Pin, LOW); // Turn off LED4
}

void loop() {
  // Read the state of the switch
  bool currentSwitchState = digitalRead(switchPin) == LOW;

  if (currentSwitchState != switchState) {
    // If the switch state has changed
    switchState = currentSwitchState;
    if (!switchState) {
      // If the switch is turned off, allow reading to resume
      stopReading = false;
    }
  }

  if (switchState && !stopReading) { // If switch is ON and reading is not stopped
    // Read the value from the potentiometer
    int sensorValue = analogRead(potPin);

    // Map the sensor value from 0-1023 to 15-35
    int outputValue = map(sensorValue, 0, 1023, 10, 35);

    // Control LEDs based on potentiometer reading
    if (outputValue >= 20 && outputValue <= 25) {
      digitalWrite(led2Pin, HIGH);
      digitalWrite(led3Pin, LOW);
      digitalWrite(led4Pin, LOW);
    } else if (outputValue >= 26 && outputValue <= 31) {
      digitalWrite(led2Pin, LOW);
      digitalWrite(led3Pin, HIGH);
      digitalWrite(led4Pin, LOW);
    } else if (outputValue >= 32 && outputValue <= 42) {
      digitalWrite(led2Pin, LOW);
      digitalWrite(led3Pin, LOW);
      digitalWrite(led4Pin, HIGH);
    } else {
      digitalWrite(led2Pin, LOW); // Turn off LED
      digitalWrite(led3Pin, LOW);
      digitalWrite(led4Pin, LOW);
    }

    // Send the output value over Bluetooth
    BTSerial.println(outputValue);
    Serial.print("Temperature: ");
    Serial.println(outputValue);

    delay(100); // Delay for stability
  }

  // Check for incoming Bluetooth commands to control LEDs
  if (Serial.available() > 0) {
    msg = Serial.readStringUntil('\n'); // Read until newline character

    // Split the received message into command and duration
    int commaIndex = msg.indexOf(',');
    if (commaIndex != -1) { // Check if comma exists
      // Extract command and duration substrings
      String command = msg.substring(0, commaIndex);
      String durationStr = msg.substring(commaIndex + 1);

      // Convert duration string to integer
      duration = durationStr.toInt();

      if (command == "1") {
        // Turn on LED1
        digitalWrite(led1Pin, HIGH);
        previousMillis = millis(); // Store the current time
      } else if (command == "2") {
      // Turn off LED2, LED3, and LED4, and stop reading the potentiometer
      stopReading = true;
    }else if (command == "3") {
      stopReading = false;
    } else if (command == "4") {
      
      // Turn off LED2, LED3, and LED4, and stop reading the potentiometer
      digitalWrite(led2Pin, LOW);
      digitalWrite(led3Pin, LOW);
      digitalWrite(led4Pin, LOW);
      delay(2000);
    }}
  }

  // Check if it's time to turn off LED1
  if (digitalRead(led1Pin) == HIGH && millis() - previousMillis >= duration) {
    digitalWrite(led1Pin, LOW); // Turn off LED1
  }
}
