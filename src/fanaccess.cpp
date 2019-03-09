#include "fanaccess.h"
#include <stdio.h>
#include <Arduino.h>

#define MAX_MESSAGE_SIZE 1024
#define MAX_JSON_SIZE 512

void FanAccess::init(const char *device_id, const char *enc_key, bool enc_en)
{
    this->device_id = device_id;
    enc.set_key((const unsigned char *)enc_key, enc_en);
    snprintf(platform_topic, sizeof(platform_topic), "/%s/p2d", device_id);
}

bool FanAccess::process_received_message(char *topic, char *payload, int msg_size)
{
    if (strcmp(topic, platform_topic) != 0)
    {
        // Message dose not belong to this device
        return false;
    }

    char buffer[MAX_MESSAGE_SIZE];
    int size = enc.dec(payload, msg_size, buffer);


    if (size<=0) {
        // ToDo: Generate log for invalid message
        return false;
    }

    StaticJsonDocument<MAX_JSON_SIZE> jb;
    DeserializationError err = deserializeJson(jb, buffer, size);

    if (err) {
        Serial.printf("%s: message error: %s\n",device_id, err.c_str());
        return false;
    } 

    JsonObject obj = jb.as<JsonObject>();
    JsonObject data = obj["data"][0];

    if (data.isNull()) {
        Serial.printf("%s: error. \"DATA\" is missed!\n", device_id);
        return false;
    }

    for(JsonPair cmd: data) {
        field_callback(cmd, field_callback_data);
    }
    return true;
}



bool FanAccess::send_to_platform(field_data_t * datalist, int timestamp) {
    StaticJsonDocument<MAX_JSON_SIZE> doc;
    char buffer[MAX_MESSAGE_SIZE];

    JsonArray data_array = doc.createNestedArray("data");
    
    JsonObject data = data_array.createNestedObject();


    for(int i=0; datalist[i].name; i++) {
        add_to_json(data, datalist[i]);
    }

    doc["TimeStamp"] = timestamp;

    int json_size = serializeJson(doc, buffer, sizeof(buffer));

    char msg[MAX_MESSAGE_SIZE];
    int msg_size = enc.enc(buffer, json_size, msg);
    msg[msg_size]=0; // Create a valid null terminated string


    char topic[MAX_TOPIC_LEN];
    snprintf(topic,sizeof(topic),"/%s/d2p",device_id);

    return pub_func(topic, msg);
}

void FanAccess::add_to_json(JsonObject & data, field_data_t & fieldinfo) {
    switch (fieldinfo.data_type)
    {
        case field_data_t::STR:
            data[fieldinfo.name] = (char *) fieldinfo.value;
            break;
        case field_data_t::NUM:
            data[fieldinfo.name] = (int) fieldinfo.value;
            break;
        case field_data_t::UNUM:
            data[fieldinfo.name] = (unsigned int) fieldinfo.value;
            break;
    }
}
