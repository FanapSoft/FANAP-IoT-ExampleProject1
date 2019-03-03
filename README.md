# FANAP-IoT-ExampleProject1
Example Arduino Project for using Fanap IoT Platform

## Requirements
- ESP32 development board
- Arduino IDE (tested with v1.8.8)
- Libraries:
  - WiFi
  - ArduinoJson
  - PubSubClient

NOTE: Change `MQTT_MAX_PACKET_SIZE` to 512 in `PubSubClient.h`. Modify library in `Arduino/libraries` folder or add modifed library to the 
root directory of the project.