#include <NimBLEDevice.h>
#include <FastLED.h>

// Device configuration
#define DEVICE_NAME "Soil Moisture Sensor"
#define BLINK_LED true
#define DEBUG false  // Set to false for battery deployment
#define NUM_LEDS 1

#define LED_PIN 8        // GPIO8 - LED/Logging pin
#define MOIST_ADC_PIN 2  // GPIO2 - ADC1_CH1
#define BAT_ADC_PIN 1    // GPIO1 - ADC1_CH0
#define MOIST_PWR_PIN 10 // GPIO22

// Calibration values (from your testing)
#define MOISTURE_SENSOR_READING_IN_AIR 2048   // Dry reading
#define MOISTURE_SENSOR_READING_IN_WATER 784  // Wet reading
#define VOLTAGE_DIVIDER_RESISTOR_1_KO 100     // Connected to Vcc
#define VOLTAGE_DIVIDER_RESISTOR_2_KO 98      // Connected to GND

// BTHome V2 Object IDs
#define BTHOME_BATTERY 0x01
#define BTHOME_VOLTAGE 0x0C
#define BTHOME_MOISTURE 0x14

// Debug macros
#if DEBUG
  #define DEBUG_BEGIN(baud) Serial.begin(baud); delay(2000)
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif


CRGB leds[NUM_LEDS];
BLEAdvertising* pAdvertising;

void setup() {
  DEBUG_BEGIN(115200);
  DEBUG_PRINTLN("\n\n=== Soil Moisture Sensor Starting ===");
  
  pinMode(MOIST_PWR_PIN, OUTPUT);
  digitalWrite(MOIST_PWR_PIN, LOW);

  analogSetPinAttenuation(BAT_ADC_PIN, ADC_11db);
  analogSetPinAttenuation(MOIST_ADC_PIN, ADC_11db);

  if (BLINK_LED) {
    DEBUG_PRINTLN("Initializing LED...");
    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(8);
    DEBUG_PRINTLN("LED initialized");
  }

  // Blink LED post restart
  esp_reset_reason_t reason = esp_reset_reason();
  DEBUG_PRINT("Reset reason: ");
  DEBUG_PRINTLN(reason);
  
  if (reason == ESP_RST_EXT || reason == ESP_RST_POWERON || reason == ESP_RST_DEEPSLEEP) {
    if (BLINK_LED) {
      blinkLED();
    }
  }

  // Initialize BLE
  DEBUG_PRINTLN("Initializing BLE...");
  BLEDevice::init(DEVICE_NAME);
  DEBUG_PRINTLN("BLE initialized");

  // Read sensor values
  DEBUG_PRINTLN("\n--- Reading Sensors ---");
  float moisture = readMoistureLevel();
  float voltage = readBatteryVoltage();
  uint8_t battery = getBatteryRemainingPercentage(voltage);

  DEBUG_PRINT("Moisture: ");
  DEBUG_PRINT(moisture);
  DEBUG_PRINTLN("%");
  
  DEBUG_PRINT("Voltage: ");
  DEBUG_PRINT(voltage);
  DEBUG_PRINTLN("V");
  
  DEBUG_PRINT("Battery: ");
  DEBUG_PRINT(battery);
  DEBUG_PRINTLN("%");

  // Advertise BTHome data
  DEBUG_PRINTLN("\n--- Starting BLE Advertising ---");
  advertiseBTHome(moisture, battery, voltage);
  DEBUG_PRINTLN("Advertising complete");

  // Enter deep sleep for 5 Minutes
  const uint64_t SLEEP_TIME = 5ULL * 60ULL * 1000000ULL;
  DEBUG_PRINT("\nEntering deep sleep for ");
  DEBUG_PRINT(SLEEP_TIME / 1000000ULL);
  DEBUG_PRINTLN(" seconds...");
  DEBUG_PRINTLN("=====================================\n");
  
  #if DEBUG
    delay(100);  // Give time for serial to finish
  #endif
  
  esp_sleep_enable_timer_wakeup(SLEEP_TIME);
  esp_deep_sleep_start();
}

void loop() {}

void blinkLED() {
  DEBUG_PRINTLN("LED: Blinking...");
  leds[0] = CRGB::White;
  FastLED.show();
  delay(1000);
  leds[0] = CRGB::Black;
  FastLED.show();
}

float readMoistureLevel() {
  DEBUG_PRINTLN("Reading moisture sensor...");
  digitalWrite(MOIST_PWR_PIN, HIGH);
  delay(500);
  
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    int reading = analogRead(MOIST_ADC_PIN);
    sum += reading;
    #if DEBUG
      Serial.print("  ADC[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(reading);
    #endif
    delay(50);
  }
  digitalWrite(MOIST_PWR_PIN, LOW);

  int raw = sum / 10;
  DEBUG_PRINT("Moisture raw average: ");
  DEBUG_PRINTLN(raw);

  if (raw < 500 || raw > 4000) {
    DEBUG_PRINTLN("WARNING: Moisture reading out of range!");
    return 0;
  }
  
  int percent = map(raw, MOISTURE_SENSOR_READING_IN_AIR, MOISTURE_SENSOR_READING_IN_WATER, 0, 100);
  if (percent < 0) percent = 0;
  if (percent > 100) percent = 100;
  
  DEBUG_PRINT("Moisture percent: ");
  DEBUG_PRINT(percent);
  DEBUG_PRINTLN("%");
  
  return percent;
}

void advertiseBTHome(float moisture, uint8_t battery, float voltage) {
  uint16_t moist = (uint16_t)(moisture * 100);
  uint16_t volt = (uint16_t)(voltage * 1000);

  DEBUG_PRINT("BTHome - Moisture: ");
  DEBUG_PRINT(moist);
  DEBUG_PRINT(", Battery: ");
  DEBUG_PRINT(battery);
  DEBUG_PRINT("%, Voltage: ");
  DEBUG_PRINT(volt);
  DEBUG_PRINTLN("mV");

  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->reset();  // Clear previous data
  
  BLEAdvertisementData advertisementData;

  // Build BTHome service data
  std::string serviceData;
  serviceData += (char)0x40;  // BTHome header

  serviceData += (char)BTHOME_BATTERY;
  serviceData += (char)battery;

  serviceData += (char)BTHOME_VOLTAGE;
  serviceData += (char)(volt & 0xFF);
  serviceData += (char)((volt >> 8) & 0xFF);

  serviceData += (char)BTHOME_MOISTURE;
  serviceData += (char)(moist & 0xFF);
  serviceData += (char)((moist >> 8) & 0xFF);

  #if DEBUG
    Serial.print("  Service data (");
    Serial.print(serviceData.length());
    Serial.print(" bytes): ");
    for (size_t i = 0; i < serviceData.length(); i++) {
      Serial.print("0x");
      if ((uint8_t)serviceData[i] < 16) Serial.print("0");
      Serial.print((uint8_t)serviceData[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  #endif

  // IMPORTANT: Set service data with the UUID
  BLEUUID serviceUUID((uint16_t)0xFCD2);
  advertisementData.setServiceData(serviceUUID, serviceData);
  
  // Set other advertisement data
  advertisementData.setFlags(0x06);
  advertisementData.setCompleteServices(serviceUUID);
  advertisementData.setName(DEVICE_NAME);

  // Apply to advertising
  pAdvertising->setAdvertisementData(advertisementData);

  // Advertise multiple times
  for (int i = 0; i < 3; i++) {
    DEBUG_PRINT("Advertising packet ");
    DEBUG_PRINT(i + 1);
    DEBUG_PRINTLN("/3");
    pAdvertising->start();
    delay(1000);
    pAdvertising->stop();
    delay(200);
  }
  
  DEBUG_PRINTLN("BLE advertising stopped");
}

float readBatteryVoltage() {
  DEBUG_PRINTLN("Reading battery voltage...");
  long sum = 0;

  for (int i = 0; i < 10; i++) {
    int reading = analogRead(BAT_ADC_PIN);
    sum += reading;
    #if DEBUG
      Serial.print("  ADC[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(reading);
    #endif
    delay(10);
  }

  int rawValue = sum / 10;
  float adcVoltage = (rawValue / 4095.0) * 3.3;
  
  // Calculate battery voltage using voltage divider formula: Vbat = Vadc * (R1+R2)/R2
  float multiplier = (float)(VOLTAGE_DIVIDER_RESISTOR_1_KO + VOLTAGE_DIVIDER_RESISTOR_2_KO) / VOLTAGE_DIVIDER_RESISTOR_2_KO;
  float batteryVoltage = adcVoltage * multiplier;

  DEBUG_PRINT("Battery raw average: ");
  DEBUG_PRINTLN(rawValue);
  DEBUG_PRINT("ADC voltage: ");
  DEBUG_PRINT(adcVoltage);
  DEBUG_PRINTLN("V");
  DEBUG_PRINT("Multiplier: ");
  DEBUG_PRINTLN(multiplier);
  DEBUG_PRINT("Battery voltage (before clamping): ");
  DEBUG_PRINT(batteryVoltage);
  DEBUG_PRINTLN("V");

  if (batteryVoltage > 4.3) {
    batteryVoltage = 4.2;
    DEBUG_PRINTLN("WARNING: Battery voltage clamped to 4.2V");
  }
  if (batteryVoltage < 2.8) {
    batteryVoltage = 3.0;
    DEBUG_PRINTLN("WARNING: Battery voltage clamped to 3.0V");
  }
  
  return batteryVoltage;
}

uint8_t getBatteryRemainingPercentage(float voltage) {
  const float MIN_VOLTAGE = 3.0;  // 0%
  const float MAX_VOLTAGE = 4.2;  // 100%

  int percent = (int)((voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE) * 100);
  return constrain(percent, 0, 100);
}
