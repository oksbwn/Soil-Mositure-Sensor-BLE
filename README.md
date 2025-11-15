# 🌱 BLE Soil Moisture Sensor

A low-power, battery-operated soil moisture monitoring system using ESP32-H2 with Bluetooth Low Energy connectivity. Designed for IoT garden and agriculture applications requiring real-time soil moisture data with extended battery life.

![Enclosure](.img/Enclosure.png)

## 📋 Overview

This project implements a wireless soil moisture sensor using capacitive sensing technology and Bluetooth Low Energy (BLE) communication. The ESP32-H2's ultra-low power capabilities combined with deep sleep modes enable months of operation on a single 350mAh battery, making it ideal for remote monitoring applications.

## ✨ Features

- **Low Power Consumption**: Deep sleep modes with BLE wake-up capability
- **Accurate Moisture Sensing**: Capacitive sensing prevents corrosion unlike resistive sensors
- **Wireless Connectivity**: Bluetooth 5.3 LE for reliable data transmission
- **Long Range**: Up to -106.5 dBm receiver sensitivity
- **Compact Design**: Battery-powered portable enclosure
- **Matter/Thread Ready**: ESP32-H2 supports future protocol expansion

## 🔧 Components

### 1. ESP32-H2 Microcontroller

The ESP32-H2 is an ultra-low-power System-on-Chip (SoC) specifically designed for IoT applications.

**Key Specifications:**
- **Processor**: 32-bit RISC-V single-core @ 96 MHz
- **Wireless**: Bluetooth 5.3 LE certified, IEEE 802.15.4 (Thread/Zigbee/Matter)
- **Radio Performance**: 
  - BLE receiver sensitivity: up to -106.5 dBm
  - Supports 1 Mbps and 2 Mbps PHY
  - Long range Coded PHY (125/500 Kbps)
- **Power**: Designed for ultra-low power consumption
- **Frequency**: 2.4 GHz band
- **Security**: Built-in security features for connected devices

**Why ESP32-H2?**
The ESP32-H2 excels in battery-powered applications due to its advanced power management, deep sleep modes, and efficient BLE 5.3 implementation.

### 2. Capacitive Soil Moisture Sensor

Capacitive soil moisture sensors measure soil moisture by detecting changes in dielectric permittivity of the surrounding medium.

**Key Specifications:**
- **Operating Voltage**: 3.3V - 5.5V DC
- **Current Consumption**: ~5mA
- **Output Signal**: Analog (0 - 3.0V DC)
- **Accuracy**: ±3% (depends on soil type and calibration)
- **Material**: FR4 (non-corrosive, durable)
- **Connector**: PH 2.54-3P
- **Dimensions**: 98mm × 23mm × 4mm

**Advantages over Resistive Sensors:**
- No metal contact with soil prevents corrosion
- Longer lifespan in harsh soil conditions
- More accurate moisture readings

### 3. 350mAh Battery

A compact lithium battery providing portable power for extended outdoor monitoring.

**Specifications:**
- **Capacity**: 350mAh
- **Type**: Lithium Polymer (LiPo) recommended
- **Voltage**: 3.7V nominal
- **Expected Runtime**: Weeks to months depending on sampling frequency and deep sleep configuration

## 🔌 Hardware Connections

## Libraries

1. https://github.com/deeja/BTHomeV2-Arduino
2. https://github.com/h2zero/NimBLE-Arduino