#ifndef _DES_H_
#define _DES_H_

#define DES_KEY_SIZE 8

#define ENCRYPTION_MODE 1
#define DECRYPTION_MODE 0


typedef struct {
	unsigned char k[8];
	unsigned char c[4];
	unsigned char d[4];
} key_set;


class DES
{
public:

    // Build DES key from string. Minimum size of key is 8bytes. 
    // If input key is shorter than 8 missing bytes will be filled by \0
    void set_key(const unsigned char * main_key);

    int des_block(const unsigned char* msg, int msg_size, unsigned char * out_buffer, int mode);

private:

    void generate_sub_keys(unsigned char* main_key, key_set* key_sets);

    int des_blk(const unsigned char* msg, int msg_size, unsigned char * out_buffer, int mode, int finalize);

    void process_message(const unsigned char* message_piece, unsigned char* processed_piece, key_set* key_sets, int mode);

    key_set gkeys[17];
};


#endif


