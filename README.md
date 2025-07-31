# 🌱 Smart Soil Moisture Checker with Mobile Monitoring

This project uses two ESP32 modules to monitor soil moisture, temperature, and humidity in real time and simulate irrigation control. It communicates sensor data via MQTT and displays readings on a live mobile-friendly dashboard.

📌 What it Does

- Reads soil moisture using a capacitive sensor  
- Measures temperature and humidity using a DHT11 sensor  
- Publishes sensor data as JSON to an MQTT topic (`esp32/soilSensorData`)  
- A second ESP32 subscribes to this topic and:
  - Turns ON an LED if soil is too dry (simulating pump activation)
  - Turns OFF the LED when moisture is sufficient
- Displays real-time sensor data on a mobile dashboard
- Designed to be low-cost, modular, and energy-efficient


🔧 Hardware
ESP32 board (x2)             
Capacitive Soil Moisture Sensor
DHT11 Sensor                   
LED (Pump Simulation)          
Power Supply (USB)             
Wi-Fi Network (Hospot or Wifi)               

🛰️ Communication & Protocol

- MQTT Protocol using broker  
- JSON-formatted payloads for data consistency  
- Real-time publishing and subscribing between ESP32 nodes

---

📲 Dashboard

- Real-time visualization of:
  - Soil Moisture
  - Temperature
  - Humidity  
- Future enhancements planned: notifications, charts, history logs

🛠️ Future Improvements

- Real pump control via relay
- Solar-powered field deployment
- Historical data logging & analysis
- Multi-node support for large field monitoring



