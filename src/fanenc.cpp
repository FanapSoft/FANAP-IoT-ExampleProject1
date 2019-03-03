#include "fanenc.h"

#include <stdlib.h>
#include <string.h>

int FanEnc::dec(char * msg, int msg_size, char * buffer) {

  if (!enc_en) {
    memcpy(buffer, msg, msg_size);
    return msg_size;
  }

  unsigned char * temp_buff;

  temp_buff = (unsigned char *) malloc(msg_size);

  if (!temp_buff) {
    return 0;
  }

  int b64_size = base64_decode(msg, msg_size, temp_buff);

  if (b64_size<=0) {
    free(temp_buff);
    return 0;
  }

  int dec_size = des_enc.des_block((const unsigned char*) temp_buff, b64_size, (unsigned char *) buffer, DECRYPTION_MODE);

  free(temp_buff);

  return dec_size;
}




// Encrypt message for sending to the Fan-IOT platform
int FanEnc::enc(char * msg, int msg_size, char * buffer) {

  if (!enc_en) {
    memcpy(buffer, msg, msg_size);
    return msg_size;
  }

  unsigned char * temp_buff;

  temp_buff = (unsigned char * )malloc(msg_size+DES_KEY_SIZE);

  if (!temp_buff) {
    return 0;
  }

  int enc_size = des_enc.des_block((const unsigned char*)msg, msg_size, (unsigned char *) temp_buff, ENCRYPTION_MODE);


  int b64_size = base64_encode(temp_buff, enc_size, buffer);

  free(temp_buff);

  return b64_size;
}
