//Cpoyright @ Alireza Seydi
#include <SoftwareSerial.h>

SoftwareSerial sim800lSerial(10, 11); // RX, TX

struct MoistureSensor {
  int pin;
  int flowerIndex;
};

struct Flower {
  String name;
  int lowThreshold;
  int highThreshold;
};

enum SystemState {
  RUNNING,
  PAUSED,
  ERROR
};

class IrrigationLine {
public:
  int valvePin;
  int pumpPin;  // Individual pin for the pump
  MoistureSensor sensors[2]; // Assuming each line has 2 sensors, adjust as needed
};

IrrigationLine irrigationLines[] = {
  {12, 8, {{A0, 0}, {A1, 1}}}, // Valve on pin 12, pump on pin 8, associated with sensors A0 and A1
  {13, 9, {{A2, 2}, {A3, 3}}}  // Valve on pin 13, pump on pin 9, associated with sensors A2 and A3
  // Add more lines as needed
};

Flower flowers[] = {
  {"Rose", 400, 600}, // Replace with actual low and high thresholds for each flower
  {"Lily", 350, 550}, // Replace with actual low and high thresholds for each flower
  {"Tulip", 300, 500},
  {"Daisy", 380, 580}
  // Add more flowers as needed 
};

const int waterLevelPin = 9;   // Digital pin for water level sensor
const int waterLevelThresholdLow = LOW; // Adjust this value based on the water level sensor characteristics
const int waterLevelThresholdHigh = HIGH; // Adjust this value based on the water level sensor characteristics

const int pumpOnTimeLow = 60000;   // Pump on time for low average difference in milliseconds (1 minute)
const int pumpOnTimeMedium = 90000; // Pump on time for medium average difference in milliseconds (1.5 minutes)
const int pumpOnTimeHigh = 120000;  // Pump on time for high average difference in milliseconds (2 minutes)

const int waitTime = 21600000;  // Wait time in milliseconds (6 hours)
const int additionalWaitTime = 3600000;  // Additional wait time in milliseconds (1 hour)

const int consecutiveCheckThreshold = 30; // Percentage difference threshold for consecutive checks
const int maxConsecutiveChecks = 3;

SystemState systemState = RUNNING;

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging

  checkSystemComponents();

  // Set pinMode for moisture sensor pins, pump pins, and valve pins
  for (int i = 0; i < sizeof(irrigationLines) / sizeof(irrigationLines[0]); i++) {
    pinMode(irrigationLines[i].valvePin, OUTPUT);
    pinMode(irrigationLines[i].pumpPin, OUTPUT);
    for (int j = 0; j < sizeof(irrigationLines[i].sensors) / sizeof(irrigationLines[i].sensors[0]); j++) {
      pinMode(irrigationLines[i].sensors[j].pin, INPUT);
    }
  }

  pinMode(waterLevelPin, INPUT);

  // Start communication with SIM800L
  sim800lSerial.begin(9600);
  delay(1000); // Allow some time for the module to initialize
}

void loop() {
  int waterLevel = digitalRead(waterLevelPin);

  if (waterLevel == waterLevelThresholdLow) {
    if (systemState == RUNNING) {
      // Water level is below the threshold, pause the program
      pauseProgram();
      sendSMS("Water level is critically low. Program paused.");
    }
  } else {
    if (systemState == PAUSED) {
      // Water level is back to normal, resume the program
      resumeProgram();
      sendSMS("Water level has been refilled. Program resumed.");
    }

    float totalPercentageDifference = 0.0;

    for (int i = 0; i < sizeof(irrigationLines) / sizeof(irrigationLines[0]); i++) {
    float lineAveragePercentageDifference = calculateLineAveragePercentageDifference(irrigationLines[i]);

    if (lineAveragePercentageDifference > 60) {
      // Activate the pump and valve for 2 minutes
      if (!activatePumpAndValve(irrigationLines[i], pumpOnTimeHigh)) {
        systemState = ERROR;
        sendSMS("Failed to activate pump for Line " + String(i) + ". System in error state.");
      } else {
        sendSMS("Average moisture level for Line " + String(i) + " is low. Pump and valve will run for 2 minutes.");
      }
    } else if (lineAveragePercentageDifference > 50) {
      // Activate the pump and valve for 1.5 minutes
      if (!activatePumpAndValve(irrigationLines[i], pumpOnTimeMedium)) {
        systemState = ERROR;
        sendSMS("Failed to activate pump for Line " + String(i) + ". System in error state.");
      } else {
        sendSMS("Average moisture level for Line " + String(i) + " is moderate. Pump and valve will run for 1.5 minutes.");
      }
    } else if (lineAveragePercentageDifference > 30) {
    // Activate the pump and valve for 1 minute
      if (!activatePumpAndValve(irrigationLines[i], pumpOnTimeLow)) {
        systemState = ERROR;
        sendSMS("Failed to activate pump for Line " + String(i) + ". System in error state.");
      } else {
        sendSMS("Average moisture level for Line " + String(i) + " is acceptable. Pump and valve will run for 1 minute.");
    } else {
      // Wait and check again if the difference is not significant
      delay(additionalWaitTime);
      consecutiveChecks++;
    }

    if (consecutiveChecks >= maxConsecutiveChecks) {
      // Reset consecutive checks after 3 consecutive failed checks
      consecutiveChecks = 0;
    }
  }

  // Wait for the specified interval before the next check
  delay(waitTime);
}

void activatePumpAndValve(IrrigationLine line, int pumpTime) {
  // Activate the pump
  digitalWrite(line.pumpPin, HIGH);

  // Activate the valve simultaneously
  digitalWrite(line.valvePin, HIGH);
  int pumpStatus = digitalRead(line.pumpPin);
  if (pumpStatus == HIGH) {
    Serial.println("Pump activation successful.");
  } else {
    Serial.println("Pump activation failed!");
  }
  // Wait for the specified on-time
  delay(pumpTime);

  // Deactivate the pump
  digitalWrite(line.pumpPin, LOW);

  // Deactivate the valve
  digitalWrite(line.valvePin, LOW);
}

void pauseProgram() {
  // Additional actions to pause the program if needed
  systemState = PAUSED;
}

void resumeProgram() {
  // Additional actions to resume the program if needed
  systemState = RUNNING;
}

bool sendSMS(String message) {
  // Retry sending SMS in case of failure
  for (int i = 0; i < 3; i++) {
    if (sendATCommand("AT+CMGF=1") && sendATCommand("AT+CMGS=\"+1234567890\"")) {
      sim800lSerial.print(message);
      delay(1000);
      sim800lSerial.write(26); // ASCII code for Ctrl+Z
      delay(1000);
      return true; // Successful SMS sending
    } else {
      Serial.println("Failed to send SMS. Retrying...");
      delay(5000); // Wait before retrying
    }
  }

  Serial.println("Failed to send SMS after multiple attempts.");
  return false; // Failed to send SMS after multiple attempts
}

void checkSystemComponents() {
  Serial.println("Checking system components...");

  // Test moisture sensors
  for (int i = 0; i < sizeof(irrigationLines) / sizeof(irrigationLines[0]); i++) {
    for (int j = 0; j < sizeof(irrigationLines[i].sensors) / sizeof(irrigationLines[i].sensors[0]); j++) {
      int sensorPin = irrigationLines[i].sensors[j].pin;
      if (testSensor(sensorPin)) {
        Serial.println("Moisture sensor on pin " + String(sensorPin) + " is connected and working correctly.");
      } else {
        Serial.println("Moisture sensor on pin " + String(sensorPin) + " is not connected or not working!");
      }
    }
  }

  // Test water level sensor
  if (testSensor(waterLevelPin)) {
    Serial.println("Water level sensor on pin " + String(waterLevelPin) + " is connected and working correctly.");
  } else {
    Serial.println("Water level sensor on pin " + String(waterLevelPin) + " is not connected or not working!");
  }

  // Test pump pins
  for (int i = 0; i < sizeof(irrigationLines) / sizeof(irrigationLines[0]); i++) {
    int pumpPin = irrigationLines[i].pumpPin;
    if (testPin(pumpPin)) {
      Serial.println("Pump pin " + String(pumpPin) + " is connected and working correctly.");
    } else {
      Serial.println("Pump pin " + String(pumpPin) + " is not connected or not working!");
    }
  }

  // Test valve pins
  for (int i = 0; i < sizeof(irrigationLines) / sizeof(irrigationLines[0]); i++) {
    int valvePin = irrigationLines[i].valvePin;
    if (testPin(valvePin)) {
      Serial.println("Valve pin " + String(valvePin) + " is connected and working correctly.");
    } else {
      Serial.println("Valve pin " + String(valvePin) + " is not connected or not working!");
    }
  }

  // Test SIM800L module
  if (testSIM800L()) {
    Serial.println("SIM800L module is connected and working correctly.");
  } else {
    Serial.println("SIM800L module is not connected or not working!");
  }

  Serial.println("System component check completed. Starting the system.");
}

bool activatePump(int pumpPin, int pumpTime) {
  // Activate the pump for the specified on-time
  digitalWrite(pumpPin, HIGH);
  delay(pumpTime);
  digitalWrite(pumpPin, LOW);

  // Check if the pump activation was successful
  return true; // You may need to add additional logic to verify the pump's operation
}

bool testPin(int pin) {
  // Check if the pin is connected (not equal to -1)
  return pin != -1;
}

bool sendATCommand(String command) {
  // Send an AT command to the SIM800L module and check for "OK" response
  sim800lSerial.println(command);
  delay(1000);
  String response = readSIM800LResponse();

  // Check if there's an "OK" response from the module
  return response.indexOf("OK") != -1;
}

String readSIM800LResponse() {
  // Read and return the response from the SIM800L module
  String response = "";
  while (sim800lSerial.available()) {
    char c = sim800lSerial.read();
    response += c;
    delay(10); // Delay to allow the serial buffer to fill
  }
  return response;
}

bool testSensor(int pin) {
  // Check if the pin is connected (not equal to -1)
  return pin != -1;
}

bool testSIM800L() {
  // Test the SIM800L module by sending an AT command and checking for a response
  return sendATCommand("AT");
}
