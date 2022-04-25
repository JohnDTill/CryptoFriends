#ifndef DECRYPT_H
#define DECRYPT_H

#include <cassert>
#include <cstring>
#include <string>

#include <openssl/aes.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

namespace CryptoFriends {

std::string decrypt(const std::string& encryped_src, std::string_view key){
    //EVENTUALLY: stop using deprecated functions

    //here iv default character set to all 0
    unsigned char iv[AES_BLOCK_SIZE] = { '0','0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };

    AES_KEY aes_key;
    if (AES_set_decrypt_key((const unsigned char*)key.data(), key.length() * 8, &aes_key) < 0)
        assert(false);
    std::string strRet;
    for (unsigned int i = 0; i < encryped_src.length() / AES_BLOCK_SIZE; i++){
        std::string str16 = encryped_src.substr(i*AES_BLOCK_SIZE, AES_BLOCK_SIZE);
        unsigned char out[AES_BLOCK_SIZE];
        ::memset(out, 0, AES_BLOCK_SIZE);
        AES_decrypt((const unsigned char*)str16.c_str(), out, &aes_key);
        strRet += std::string((const char*)out, AES_BLOCK_SIZE);
    }

    return strRet;
}

}

#endif // DECRYPT_H
