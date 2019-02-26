#ifndef _FANENC_H_
#define _FANENC_H_

#include "des.h"
#include "base64.h"

class FanEnc
{
private:
    DES des_enc;

public:
    void set_key(const unsigned char*main_key) {
        des_enc.set_key(main_key);
    }

    // Encrypt message for sending to the Fan-IOT platform
    int enc(char * msg, int msg_size, char * buffer);

    // Decrypt message received from fan-iot platform
    int dec(char * msg, int msg_size, char * buffer);

    // Get required buffer size for message encryption
    static int get_enc_size(int msg_size) {
        // First calc size of the DES64
        int outsize = msg_size + ( DES_KEY_SIZE - msg_size%DES_KEY_SIZE);
        return (BASE64_ENCODE_OUT_SIZE(outsize)) + 1;
    }

};

#endif
