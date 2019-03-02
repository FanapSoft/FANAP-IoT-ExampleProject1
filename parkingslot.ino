
#include <PubSubClient.h>
#include <WiFi.h>

#include "src/parkingcontroller.h"



ParkingController pc;

char *device_id_list[] = {"ID-01", "ID-02", "ID-03", "ID-04"};
char *enc_key_list[] = {"KEY1", "KEY2", "KEY3", "KEY4"};
bool enc_en_list[] = {true, false, false, true};
int led_pin_list[] = {2,3,4,5};
int sensor_io_list[] = {A0, A3, A6, A7};
int sensor_low_threshold_list[] = {700, 700, 700, 700};
int sensor_high_threshold_list[] = {1000, 1000, 1000, 1000};




bool test(void*data, const char*topic, const char *payload) {
    Serial.printf("Publish: topic=%s payload=%s\n",topic, payload);
}




void setup() {
    Serial.begin(115200);
    Serial.println("Hello from ParkingSlot Test");

    pc.initialize(
        device_id_list,
        enc_key_list,
        enc_en_list,
        led_pin_list,
        sensor_io_list,
        sensor_low_threshold_list,
        sensor_high_threshold_list);





    // dut.init("DEVICEID", "ENC_KEY", false, 2, A0, 600, 1000);

    // dut.set_mqtt_publish_access(test, (void*)1231);
    // char buffer[50];
    // dut.from_platform_topic(buffer);
    // Serial.println(buffer);
}


void loop() {

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