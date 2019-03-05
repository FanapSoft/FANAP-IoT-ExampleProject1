#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>

#include "src/parkingcontroller.h"

ParkingController pc;
WiFiClient espClient;
PubSubClient client(espClient);

char *device_id_list[] = {"ID-01", "ID-02", "ID-03", "ID-04", "ID-05"};
char *enc_key_list[] = {"KEY1", "KEY2", "KEY3", "KEY4", "KEY5"};
bool enc_en_list[] = {false, false, false, false, false};
int led_pin_list[] = {23, 22, 21, 19, 18};
int sensor_io_list[] = {A0, A3, A6, A7, A4};
int sensor_low_threshold_list[] = {700, 700, 700, 700};
int sensor_high_threshold_list[] = {1000, 1000, 1000, 1000};

const char *ssid = CONFIG_WIFI_SSID;
const char *password = CONFIG_WIFI_PASS;

const int mqttPort = CONFIG_MQTT_PORT;
const char *mqttServer = CONFIG_MQTT_ADDR;

void callback(char* topic, uint8_t * payload, unsigned int size) {
    pc.mqtt_callback(topic, payload, size);
}

bool subscribe(const char*topic) {
    return client.subscribe(topic);
}

bool publish(const char*topic, const char*payload) {
    return client.publish(topic, payload);
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

    pc.mqtt_subscribe(subscribe);
    pc.mqtt_publish(publish);
}

void loop()
{
    client.loop();
    pc.loop_handle();
}