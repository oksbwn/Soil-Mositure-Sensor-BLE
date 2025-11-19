#include <Arduino.h>

#define MOIST_ADC_PIN 2   // GPIO2
#define MOIST_PWR_PIN 10  // GPIO10

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  pinMode(MOIST_PWR_PIN, OUTPUT);
  analogSetPinAttenuation(MOIST_ADC_PIN, ADC_11db);
  
  Serial.println("\n\n=== SOIL MOISTURE SENSOR CALIBRATION ===");
  Serial.println("This will help you find the correct calibration values\n");
}

void loop() {
  Serial.println("\n--- Taking 20 readings ---");
  
  // Power on sensor
  digitalWrite(MOIST_PWR_PIN, HIGH);
  delay(500);  // Let sensor stabilize
  
  long sum = 0;
  int minVal = 9999;
  int maxVal = 0;
  
  // Take 20 readings
  for (int i = 0; i < 20; i++) {
    int raw = analogRead(MOIST_ADC_PIN);
    sum += raw;
    
    if (raw < minVal) minVal = raw;
    if (raw > maxVal) maxVal = raw;
    
    Serial.printf("Reading %2d: %4d\n", i+1, raw);
    delay(100);
  }
  
  // Power off sensor
  digitalWrite(MOIST_PWR_PIN, LOW);
  
  // Calculate average
  int average = sum / 20;
  
  Serial.println("\n=== RESULTS ===");
  Serial.printf("Average: %d\n", average);
  Serial.printf("Min:     %d\n", minVal);
  Serial.printf("Max:     %d\n", maxVal);
  Serial.printf("Range:   %d\n", maxVal - minVal);
  Serial.println("\nUse the AVERAGE value for calibration");
  Serial.println("=====================================");
  
  // Wait 10 seconds before next reading
  Serial.println("\nWaiting 10 seconds...\n");
  delay(10000);
}
