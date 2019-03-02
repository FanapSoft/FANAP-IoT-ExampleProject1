#include "config.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "src/parkingcontroller.h"



ParkingController pc;
WiFiClient espClient;
PubSubClient client(espClient);

char *device_id_list[] = {"ID-01", "ID-02", "ID-03", "ID-04"};
char *enc_key_list[] = {"KEY1", "KEY2", "KEY3", "KEY4"};
bool enc_en_list[] = {true, false, false, true};
int led_pin_list[] = {2, 3, 4, 5};
int sensor_io_list[] = {A0, A3, A6, A7};
int sensor_low_threshold_list[] = {700, 700, 700, 700};
int sensor_high_threshold_list[] = {1000, 1000, 1000, 1000};

const char *ssid = CONFIG_WIFI_SSID;
const char *password = CONFIG_WIFI_PASS;

const int mqttPort = CONFIG_MQTT_PORT;
const char *mqttServer = CONFIG_MQTT_ADDR;



void callback(char* topic, uint8_t * payload, unsigned int size) {
    pc.mqtt_callback(topic, payload, size);
}


bool test(void *data, const char *topic, const char *payload)
{
    Serial.printf("Publish: topic=%s payload=%s\n", topic, payload);
}

void setup()
{

    Serial.begin(115200);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Connecting to the wifi....");
    }

    client.setCallback(callback);

    client.setServer(mqttServer, mqttPort);

    while (!client.connected())
    {
        Serial.println("Connecting to MQTT...");

        if (client.connect("ESP32Client"))
        {

            Serial.println("connected");
        }
        else
        {

            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

    client.subscribe("test/#");

    Serial.println("Connected!");
    Serial.println(WiFi.localIP());


    pc.init_slots(
        device_id_list,
        enc_key_list,
        enc_en_list,
        led_pin_list,
        sensor_io_list,
        sensor_low_threshold_list,
        sensor_high_threshold_list);


    // dut.set_mqtt_publish_access(test, (void*)1231);
    // char buffer[50];
    // dut.from_platform_topic(buffer);
    // Serial.println(buffer);
}

void loop()
{
    client.loop();
    pc.loop_handle();

    // while( Serial.available()>0) {
    //     char m = Serial.read();
    //     switch (m)
    //     {
    //         case '1':
    //             dut.set_led(ParkingSlot::ON);
    //             Serial.println("ON");
    //             break;
    //         case '2':
    //             dut.set_led(ParkingSlot::OFF);
    //             Serial.println("OFF");
    //             break;
    //         case '3':
    //             dut.set_led(ParkingSlot::BLINK1);
    //             Serial.println("BLINK1");
    //             break;
    //         case '4':
    //             dut.set_led(ParkingSlot::BLINK2);
    //             Serial.println("BLINK2");
    //             break;
    //         default:
    //             Serial.println("INVALID");
    //             Serial.println(m);
    //             break;
    //     }
    // }
    // dut.handle();

    // if (dut.get_sensor_state()) {
    //     dut.set_led(ParkingSlot::ON);
    // } else {
    //     dut.set_led(ParkingSlot::OFF);
    // }

    //Serial.println(dut.get_sensor_last_value());
}