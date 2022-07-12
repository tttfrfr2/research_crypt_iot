#include "random.h"

void init_seed(int *key) {
        *key = 1;
}

void update_key(int *key) {
        *key = lcg(*key);
}

void decrement_key_index(int *key_index, int key_length, int *key) {
        if (*key_index == 0) {
                *key_index = key_length - 1;
                update_key(key);
        } else {
                *key_index = *key_index - 1;
        }
}

byte gen_key_block(int *key_index, int key_length, int *key){
        byte block;
        block = 0;
        for (int i = 7; i > 0; i--) {
                block = block | ((uint8_t(*key >> *key_index) & 0x1) << i);
                decrement_key_index(key_index, key_length, key);
        }
        return block;
}

byte encrypt(char *message, int *key_index, int key_length, int *key, int MessLen) {
        for (int messInd = 0; messInd < MessLen; messInd++) {
                byte keyBlock = gen_key_block(key_index, key_length, key);
                message[messInd] = char(byte(message[messInd]) ^ keyBlock);
        }
        //M5.Lcd.println("Enc:      " + String((char *)encrypt_message) + "\n");
        return *message;
}
