#ifndef HEX_H
#define HEX_H

#include <cassert>
#include <cinttypes>

namespace CryptoFriends {

extern constexpr uint8_t BITS_PER_HEX_CHAR = 4;
extern constexpr uint8_t MAX_HEX_CHAR = (1 << BITS_PER_HEX_CHAR) - 1;

constexpr uint8_t hexCharToByte(char ch) noexcept {
    if(ch >= '0' && ch <= '9') return static_cast<uint8_t>(ch - '0');
    assert(ch >= 'a' && ch <= 'f');
    return static_cast<uint8_t>(ch - 'a' + 10);
}

constexpr char byteToHexChar(uint8_t b) noexcept {
    assert(b <= MAX_HEX_CHAR);
    if(b <= 9) return '0' + b;
    else return ('a'-10)+b;
}

}

#endif // HEX_H
