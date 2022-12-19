#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/des.h>
#include "openssl.h"

int main(int argc, char *args[])
{       

    strcpy(inputData, "20210914testdata");
      
    UC8 encrypt[1024];
   
    int encrypt_size;
    
    AES_KEY enc_key;
        
    //encrypt
    UC8 iv_aes[AES_BLOCK_SIZE];
    bzero(iv_aes, sizeof(iv_aes));
    
    AES_set_encrypt_key(cipher_key, KEY_BIT, &enc_key); //key setting
    AES_cbc_encrypt(inputData, encrypt, sizeof(inputData), &enc_key , iv_aes, AES_ENCRYPT); //encrypt
    
    FILE* fp = fopen("encrypt.txt","w");//save textfile

    fputs(encrypt, fp);
    	
    fclose(fp);
    
    funcEncrypt();
    funcDecrypt();
    
    return 0;
}

int funcEncrypt(){
    printf("\ntest Function\n");
}

int funcDecrypt(){

    AES_KEY dec_key;
    
    UC8 decrypt[1024];
    UC8 temp[1024];
    
    UC8 iv_des[AES_BLOCK_SIZE];    
    bzero(iv_des, sizeof(iv_des));
      
    FILE* fp = fopen("encrypt.txt","r");//save textfile
    
    fgets(temp,sizeof(temp),fp);
    
    fclose(fp);
    
    //decrypt case1. (copy to variable)
    //encrypt_size = (sizeof(inputData) + AES_BLOCK_SIZE)/16 * 16;
    //memcpy(temp, encrypt, encrypt_size); //sizeof(inputData))
    
    //decrypt case2.- (use file variable)
    AES_set_decrypt_key(cipher_key, KEY_BIT, &dec_key);
    AES_cbc_encrypt(temp, decrypt, sizeof(inputData), &dec_key , iv_des, AES_DECRYPT);

                
    for (int ndx = 0; ndx < sizeof(inputData); ndx++){
        //printf( "encrypt : 0x%02x\n", encrypt[ndx]);
        printf( "decrypt : 0x%02x(%c) \n", decrypt[ndx], decrypt[ndx]);
        printf("-------------------------\n");
    }
    
    printf( "decrypt : %s \n", decrypt);
}