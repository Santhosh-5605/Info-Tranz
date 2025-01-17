#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>

// Create MPU6050 object
Adafruit_MPU6050 mpu;

// GSM module pins
SoftwareSerial gsm(7, 8); // RX, TX for GSM module

// Threshold for accident detection
float accidentThreshold = 2.5; // Adjust based on calibration

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to initialize MPU6050!");
    while (1);
  }
  Serial.println("MPU6050 initialized!");

  // Setup GSM module
  sendGSMCommand("AT");
  sendGSMCommand("AT+CMGF=1"); // Set SMS mode
}

void loop() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Monitor accelerometer for sudden tilts
  if (detectAccident(a.acceleration.x, a.acceleration.y, a.acceleration.z)) {
    Serial.println("Accident detected!");
    sendEmergencyAlert(a.acceleration.x, a.acceleration.y, a.acceleration.z);
    delay(10000); // Prevent spamming alerts
  }
  delay(100);
}

bool detectAccident(float ax, float ay, float az) {
  // Calculate net acceleration
  float netAccel = sqrt(ax * ax + ay * ay + az * az);
  return netAccel > accidentThreshold;
}

void sendEmergencyAlert(float ax, float ay, float az) {
  String location = "Latitude: 12.9716, Longitude: 77.5946"; // Replace with GPS module integration
  String message = "Accident detected! \nLocation: " + location + 
                   "\nAcceleration Data:\nX: " + String(ax) + 
                   "\nY: " + String(ay) + "\nZ: " + String(az);
  
  sendGSMCommand("AT+CMGS=\"+911234567890\""); // Replace with emergency number
  gsm.print(message);
  gsm.write(26); // Ctrl+Z to send the message
  Serial.println("Emergency alert sent!");
}

void sendGSMCommand(String command) {
  gsm.println(command);
  delay(1000);
  while (gsm.available()) {
    Serial.write(gsm.read());
  }
}

