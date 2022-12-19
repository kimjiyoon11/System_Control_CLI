#define KEY_BIT 128

typedef unsigned char   UC8;

UC8 inputData[16];

static const UC8 cipher_key[AES_BLOCK_SIZE];

int funcDecrypt();
int funcEncrypt();