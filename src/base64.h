#ifndef BASE64_H
#define BASE64_H

#include <cassert>
#include <cinttypes>

namespace CryptoFriends {

extern constexpr uint8_t BITS_PER_BASE64_CHAR = 6;

constexpr uint8_t base64CharToByte(char ch) noexcept {
    if(ch >= 'A' && ch <= 'Z') return ch - 'A';
    else if(ch >= 'a' && ch <= 'z') return (ch - 'a') + 26;
    else if(ch >= '0' && ch <= '9') return (ch - '0') + 52;
    else if(ch == '+') return 62;
    assert(ch == '/');
    return 63;
}

constexpr char byteToBase64Char(uint8_t byte) noexcept {
    assert(byte < 64);
    if(byte <= 25) return static_cast<char>(byte) + 'A';
    else if(byte <= 51) return static_cast<char>(byte) + ('a' - 26);
    else if(byte <= 61) return static_cast<char>(byte) + ('0' - 52);
    else if(byte == 62) return '+';
    else return '/';
}

}

#endif // BASE64_H
