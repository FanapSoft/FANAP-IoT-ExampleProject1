#include "config.h"
#include <WiFi.h>
#include <PubSubClient.h>

#include "src/parkingcontroller.h"
#include "src/ledblinker.h"

ParkingController pc;
WiFiClient espClient;
PubSubClient client(espClient);
LedBlinker health_led;

char *device_id_list[] = {"SLOT-1", "SLOT-2", "SLOT-3", "SLOT-4"};
char *enc_key_list[] = {"KEY1", "KEY2", "KEY3", "KEY4", "KEY5"};
bool enc_en_list[] = {false, false, false, false, false};
int led_pin_list[] = {22, 23, 21, 19};

int sensor_io_list[] = {A0, A3, A6, A7};

int sensor_low_threshold_list[] = {700, 700, 700, 700, 700};
int sensor_high_threshold_list[] = {1000, 1000, 1000, 1000, 1000};


char * gate_device_id = "GATE-1";
char * gate_enc_key = "GATE-KEY";
const bool gate_enc_en = false;
const int gate_led_pin = 12;
const int entry_sensor_io = A4;
const int exist_sensor_io = A5;
const int servo_io = 14;
const int gate_sensor_low = 700;
const int gate_sensor_high = 1000;
const int gate_low = 5;
const int gate_high = 95;


const char *ssid = CONFIG_WIFI_SSID;
const char *password = CONFIG_WIFI_PASS;

const int mqttPort = CONFIG_MQTT_PORT;
const char *mqttServer = CONFIG_MQTT_ADDR;

void callback(char *topic, uint8_t *payload, unsigned int size)
{
    pc.mqtt_callback(topic, payload, size);
}

bool subscribe(const char *topic)
{
    return client.subscribe(topic);
}

bool publish(const char *topic, const char *payload)
{
    return client.publish(topic, payload);
}

void mqtt_connect(int wait_ms)
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
        delay(wait_ms);
    }
}


void setup()
{
    health_led.init(2);
    static const int blink_pattern[] = {250,-100,250,-600,0};
    health_led.set_pattern(blink_pattern);

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
        mqtt_connect(2000);
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

    pc.init_gate(
        gate_device_id,
        gate_enc_key,
        gate_enc_en,
        gate_led_pin,
        entry_sensor_io,
        exist_sensor_io,
        servo_io,
        gate_sensor_low,
        gate_sensor_high,
        gate_low,
        gate_high
    );


    pc.mqtt_subscribe(subscribe);
    pc.mqtt_publish(publish);
}

int sub_req=0;

void loop()
{

    if (!client.connected())
    {
        mqtt_connect(300);
        sub_req = 1;
    }
    else
    {
        if (sub_req) {
            sub_req = 0;
            Serial.println("Resubscribe to MQTT topics....");
            pc.mqtt_subscribe(subscribe);
        }

        client.loop();
        pc.loop_handle();
        health_led.handle();
    }
}