#ifndef _FANACCESS_H_
#define _FANACCESS_H_

#include <ArduinoJson.h>
#include "fanenc.h"

#include <Arduino.h>
#define MAX_TOPIC_LEN 80

typedef bool (*publish_t)(const char *, const char *);
typedef void (*fieldvalue_callback_t)(JsonPair field, void *data);

typedef struct {
    enum DataType {
        STR,
        NUM,
        UNUM
    };
    const char * name;
    DataType data_type;
    void * value;
} field_data_t;

class FanAccess
{
  public:

    const char *device_id;
    FanEnc enc;

    void init(const char *device_id, const char *enc_key, bool enc_en);

    bool process_received_message(char * topic, char * payload, int msg_size);

    bool send_to_platform(field_data_t * datalist, int timestamp);

    void set_callbacks(publish_t pub_func, fieldvalue_callback_t field_callback, void *field_callback_data)
    {
        this->pub_func = pub_func;
        this->field_callback = field_callback;
        this->field_callback_data = field_callback_data;
    }

    const char *from_platform_topic()
    {
        return platform_topic;
    }

  private:
    publish_t pub_func;
    fieldvalue_callback_t field_callback;
    void *field_callback_data;
    char platform_topic[MAX_TOPIC_LEN];

    void add_to_json(JsonObject & data, field_data_t & fieldinfo);
};

#endif