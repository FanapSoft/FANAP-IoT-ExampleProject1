#ifndef _PARKING_CONTROLLER_H
#define _PARKING_CONTROLLER_H

#define NUMBER_OF_PARKINGSLOT 4

#include "parkingslot.h"
#include <PubSubClient.h>

class ParkingController
{
  private:
    ParkingSlot slots[NUMBER_OF_PARKINGSLOT];
  public:

    static const int NumSlots = NUMBER_OF_PARKINGSLOT;

    void init_slots(
        char *device_id_list[],
        char *enc_key_list[],
        bool *enc_en_list,
        int *led_pin_list,
        int *sensor_io_list,
        int *sensor_low_threshold_list,
        int *sensor_high_threshold_list);

    void loop_handle();

    void mqtt_callback(char* topic, uint8_t * payload, unsigned int size);

    void mqtt_subscribe(bool(*subscribe)(const char*));

    void mqtt_publish(bool(*publish)(const char* topic, const char* msg));
};

#endif