# 🌱 BLE Soil Moisture Sensor

A low-power, battery-operated soil moisture monitoring system using ESP32-H2 with Bluetooth Low Energy connectivity and BTHome V2 protocol for seamless Home Assistant integration.

![Enclosure](.img/Enclosure.png)

## 📋 Overview

This project implements a wireless soil moisture sensor using capacitive sensing technology and Bluetooth Low Energy (BLE) communication via the BTHome V2 protocol. The ESP32-H2's ultra-low power capabilities combined with deep sleep modes enable months of operation on a single 350mAh battery.

## ✨ Features

- **BTHome V2 Protocol**: Native Home Assistant support with automatic discovery
- **Ultra-Low Power**: Deep sleep with 5-minute wake intervals
- **Tri-Sensor Monitoring**: Soil moisture, battery voltage, and battery percentage
- **Capacitive Sensing**: Corrosion-resistant, long-lasting moisture detection
- **Bluetooth 5.3 LE**: Reliable long-range wireless transmission
- **Battery Optimized**: Minimal LED brightness and sleep mode between readings
- **Compact Design**: 3D-printable enclosure

## 🔧 Hardware Components

### ESP32-H2 Super Mini
[Board Details](https://www.espboards.dev/esp32/esp32-h2-super-mini/)

- **Processor**: 32-bit RISC-V @ 96 MHz
- **Wireless**: Bluetooth 5.3 LE, IEEE 802.15.4
- **Radio**: -106.5 dBm receiver sensitivity
- **Power**: Deep sleep optimized

### Capacitive Soil Moisture Sensor

- **Operating Voltage**: 3.3V - 5.5V DC
- **Current**: ~5mA (only during readings)
- **Output**: Analog 0-3.0V
- **Material**: FR4 (corrosion-resistant)
- **Dimensions**: 98mm × 23mm × 4mm

### Power System

- **Battery**: 350mAh LiPo (3.7V nominal)
- **Voltage Divider**: 100kΩ + 100kΩ resistors
- **Filter Capacitor**: 0.1µF ceramic
- **Expected Runtime**: 2-4 months

## 🔌 Pin Configuration

| Component | GPIO Pin | Function | Notes |
|-----------|----------|----------|-------|
| Soil Moisture Sensor | GPIO2 | ADC1_CH1 | Analog input |
| Moisture Power Control | GPIO22 | Digital Out | Powers sensor during reads |
| Battery Voltage Monitor | GPIO1 | ADC1_CH0 | Via voltage divider |
| Status LED (WS2812B) | GPIO8 | Digital Out | NeoPixel indicator |

## 📡 BTHome V2 Implementation

Broadcasts using [BTHome V2 format](https://bthome.io/format/) for automatic Home Assistant discovery.

**Broadcasted Data:**
- Battery Percentage (0x01) - uint8
- Voltage (0x0C) - uint16, 0.001V precision
- Moisture (0x14) - uint16, 0.01% precision

**Device Name**: Soil Moisture Sensor  
**Service UUID**: 0xFCD2  
**Format**: V2, unencrypted

## 🚀 Setup & Installation

### 1. Hardware Assembly

**Voltage Divider Circuit:**

Battery+ ──[100kΩ]── ADC Pin (GPIO1) ──[100kΩ]── GND
|
[0.1µF Cap]
|
GND


Mount the 0.1µF capacitor as close as possible to the ESP32 ADC pin.

### 2. Software Dependencies

Install via Arduino Library Manager:
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino)
- [FastLED](https://github.com/FastLED/FastLED)

### 3. Upload Code

1. Open the .ino file in Arduino IDE
2. Select board: **ESP32-H2 Dev Module**
3. Upload Speed: 921600
4. Upload to ESP32-H2

### 4. Calibration

**Moisture Sensor:**
1. Record ADC value in dry air (~3500)
2. Submerge in water to line (~1200)
3. Update in code:
    ```cpp
    const int airValue = 3500;
    const int waterValue = 1200;
    ```

**Battery Voltage (100kΩ + 100kΩ):**
    ```cpp
    float readBatteryVoltage() {
    long sum = 0;
    for (int i = 0; i < 10; i++) {
    sum += analogRead(BAT_ADC_PIN);
    delay(10);
    }
    int rawValue = sum / 10;
    float adcVoltage = (rawValue / 4095.0) * 3.3;
    float batteryVoltage = adcVoltage * 2.0;
    if (batteryVoltage > 4.3) batteryVoltage = 4.2;
    if (batteryVoltage < 2.8) batteryVoltage = 3.0;
    return batteryVoltage;
    }
    ```

## 🏠 Home Assistant Integration

### Prerequisites

Before starting, ensure:
- Home Assistant is running (version 2022.9 or later)
- **Bluetooth integration** is enabled
- ESP32-H2 sensor is powered on and within Bluetooth range (~10m)

### Step-by-Step Onboarding

#### 1. Initial Discovery

1. Open Home Assistant web interface
2. Navigate to **Settings → Devices & Services**
3. Look for the **Discovered** section
4. Your sensor appears as **"Soil Moisture Sensor"**

#### 2. Configure the Device

1. Click **Configure** on the discovered BTHome device
2. Click **Submit**
3. Select an **Area** (e.g., "Garden", "Balcony")
4. Click **Finish**

No API keys or YAML configuration required!

#### 3. Verify Entities Created

Three entities are automatically created:
- `sensor.soil_moisture_sensor_moisture`
- `sensor.soil_moisture_sensor_battery`
- `sensor.soil_moisture_sensor_voltage`

#### 4. Add to Dashboard

1. Go to your Home Assistant dashboard
2. Click **Edit Dashboard**
3. Click **Add Card** → **Entities Card**
4. Add all three sensor entities
5. Click **Save**

### Manual Addition (If Not Auto-Discovered)

1. Go to **Settings → Devices & Services**
2. Click **BTHome** integration
3. Click **Add Entry**
4. Press reset on ESP32-H2
5. Click **Configure**

### Troubleshooting

**Sensor not discovered:**
- Ensure Bluetooth integration is active
- Check sensor within 10m
- Wait for wake cycle (5 minutes)

**Entities unavailable:**
- Check battery level
- Verify LED blinks
- Reduce distance

## ⚡ Power Optimization

**Sleep Cycle**: 5 minutes  
**Active Time**: ~6 seconds  
**LED Brightness**: 8/255 (~3%)  
**Battery Life**: 2-4 months

Disable LED for production:

#define BLINK_LED false

text

## 🔧 Configuration Options

**Adjust Sleep:**
const uint64_t SLEEP_TIME = 10ULL * 60ULL * 1000000ULL;

text

**Change Name:**
#define DEVICE_NAME "Garden Sensor 1"

text

## 📊 Schematic

![Schematic](.img/Schematics.png)

> **Note**: Use 100kΩ resistors for voltage divider. Add 0.1µF capacitor close to ADC pin.
## Caliberate the Moisture sensor

use [`moisture_sensor_calibertae.ino`](./Arduino%20Sketch/moisture_sensor_calibertae.ino) to caliberate the sensor. For me it is typicaly `2048` when sensor is in air. and `768` when dipped in air.

This would help us etup 

```cpp
#define MOISTURE_SENSOR_READING_IN_AIR 2048     // Dry reading
#define MOISTURE_SENSOR_READING_IN_WATER 768    // Wet reading
```



## 🐛 Troubleshooting

**LED not blinking:** GPIO8 may conflict - try GPIO10

**Inaccurate moisture:** Recalibrate air/water values

**Battery draining:** Reduce packets, increase sleep, disable LED

## 📚 References

- [BTHome Format](https://bthome.io/format/)
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino)
- [ESP32-H2 Board](https://www.espboards.dev/esp32/esp32-h2-super-mini/)
- [Home Assistant BTHome](https://www.home-assistant.io/integrations/bthome/)

## 📄 License

MIT License

## 🤝 Contributing

Pull requests welcome! Open an issue first to discuss changes.

---

**Made with 💚 for smart gardening**
