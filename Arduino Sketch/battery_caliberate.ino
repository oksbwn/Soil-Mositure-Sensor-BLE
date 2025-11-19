#include <Arduino.h>

#define BAT_ADC_PIN 1  // GPIO1

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  analogSetPinAttenuation(BAT_ADC_PIN, ADC_11db);
  
  Serial.println("\n\n=== BATTERY VOLTAGE CALIBRATION ===");
  Serial.println("Measure your battery with a multimeter");
  Serial.println("Compare with the calculated values below\n");
}

void loop() {
  Serial.println("\n--- Taking 20 readings ---");
  
  long sum = 0;
  int minVal = 9999;
  int maxVal = 0;
  
  // Take 20 readings
  for (int i = 0; i < 20; i++) {
    int raw = analogRead(BAT_ADC_PIN);
    sum += raw;
    
    if (raw < minVal) minVal = raw;
    if (raw > maxVal) maxVal = raw;
    
    Serial.printf("Reading %2d: %4d\n", i+1, raw);
    delay(100);
  }
  
  // Calculate average
  int average = sum / 20;
  float adcVoltage = (average / 4095.0) * 3.3;
  
  // With 100kΩ + 100kΩ divider (multiplier = 2.0)
  float batteryVoltage = adcVoltage * 2.0;
  
  Serial.println("\n=== RESULTS ===");
  Serial.printf("ADC Raw Average:  %d\n", average);
  Serial.printf("ADC Voltage:      %.3f V\n", adcVoltage);
  Serial.printf("Battery Voltage:  %.3f V (×2.0 multiplier)\n", batteryVoltage);
  Serial.printf("Min/Max Range:    %d - %d (variation: %d)\n", minVal, maxVal, maxVal - minVal);
  
  // Battery percentage
  int percent = (int)((batteryVoltage - 3.0) / (4.2 - 3.0) * 100);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  Serial.printf("Battery Percent:  %d%%\n", percent);
  
  Serial.println("\n*** MEASURE YOUR BATTERY WITH MULTIMETER ***");
  Serial.println("1. Touch multimeter probes to Battery+ and Battery-");
  Serial.println("2. Compare multimeter reading with calculated voltage above");
  Serial.println("3. If different, check your resistor values or connections");
  Serial.println("===========================================");
  
  // Wait 10 seconds before next reading
  Serial.println("\nWaiting 10 seconds...\n");
  delay(10000);
}
