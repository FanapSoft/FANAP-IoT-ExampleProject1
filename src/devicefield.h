#ifndef _DEVICEFIELD_H_
#define _DEVICEFIELD_H_


class DeviceField
{
public:
    enum DataType
    {
        STR,
        NUM
    };

    const char * name; // Name of the field. Null means last field in list
    DataType data_type;
    void * value; // Data value (unsigned int or char*)

};

#endif