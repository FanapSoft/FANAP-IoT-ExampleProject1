#include <PubSubClient.h>
#include "src/parkingslot.h"

ParkingSlot dut;


bool test(void*data, const char*topic, const char *payload) {
    Serial.printf("TEST PUBLISH!");
}




void setup() {
    Serial.begin(115200);
    Serial.println("Hello from ParkingSlot Test");

    dut.init("DEVICEID", "ENC_KEY", false, 2, A0, 600, 1000);

    dut.set_mqtt_publish_access(test, (void*)1231);
}


void loop() {


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
    dut.handle();

    if (dut.get_sensor_state()) {
        dut.set_led(ParkingSlot::ON);
    } else {
        dut.set_led(ParkingSlot::OFF);
    }

    //Serial.println(dut.get_sensor_last_value());

}