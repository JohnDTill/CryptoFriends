#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include "base64.h"
#include "hex.h"

#include <cassert>
#include <string>
#include <vector>

namespace CryptoFriends {

class ByteArray{

private:
    //Uses a vector of words to represent raw binary data
    //The vector is ordered from the least significant bits at the front to the most significant in the back
    //After an operation:
    //   The vector will have unused space. unusedBits() is never 0.

    template<typename T> static constexpr size_t byteSize() noexcept { return sizeof(T); }
    template<typename T> static constexpr size_t bitSize() noexcept { return 8*byteSize<T>(); }
    static constexpr uint8_t BYTES_PER_WORD = sizeof(size_t);
    static constexpr uint8_t BITS_PER_BYTE = 8;
    static constexpr uint8_t BITS_PER_WORD = BITS_PER_BYTE*BYTES_PER_WORD;
    static constexpr bool isBitSet(size_t word, uint8_t bit) noexcept{
        assert(bit < BITS_PER_WORD);
        return word & (size_t(1) << bit);
    }
    static constexpr char bitChar(size_t word, uint8_t bit) noexcept{
        return '0' + isBitSet(word, bit);
    }
    std::vector<size_t> data = {0};
    uint8_t used_bits = 0;
    uint8_t usedBitsInLastWord() const noexcept { return used_bits; }
    uint8_t unusedBitsInLastWord() const noexcept { return BITS_PER_WORD - used_bits; }
    void setUsedBitsInLastWord(uint8_t n_bits) noexcept {
        assert(n_bits < BITS_PER_WORD);
        used_bits = n_bits;
    }
    void setUnusedBitsInLastWord(uint8_t n_bits) noexcept {
        assert(n_bits > 0 && n_bits <= BITS_PER_WORD);
        used_bits = BITS_PER_WORD - n_bits;
    }
    size_t allocatedBits() const noexcept{ return BITS_PER_WORD*data.size(); }

public:
    size_t numBits() const noexcept{
        return allocatedBits() - unusedBitsInLastWord();
    }

    void addBit(bool set){
        data.back() |= (static_cast<size_t>(set) << usedBitsInLastWord());
        if(usedBitsInLastWord() == BITS_PER_WORD-1){
            data.push_back(0);
            setUsedBitsInLastWord(0);
        }else{
            setUsedBitsInLastWord(usedBitsInLastWord()+1);
        }
    }

    template<uint8_t N_bits> void addBits(size_t in){
        static_assert(N_bits <= BITS_PER_WORD, "N_bits cannot exceed word size");
        assert((N_bits == BITS_PER_WORD) || in < (1 << N_bits)); //Should not have upper bits set

        data.back() |= in << usedBitsInLastWord();

        if(N_bits >= unusedBitsInLastWord()){
            data.push_back(in >> unusedBitsInLastWord());
            setUsedBitsInLastWord(N_bits - unusedBitsInLastWord());
        }else{
            setUsedBitsInLastWord(usedBitsInLastWord() + N_bits);
        }
    }

    template<uint8_t N_bits, typename T = size_t> T getBits(size_t bit_index) const noexcept{
        static_assert(N_bits <= bitSize<T>(), "N_bits cannot exceed T size");
        assert(bit_index+N_bits <= allocatedBits());
        static constexpr size_t MASK =
                N_bits == bitSize<T>() ?
                std::numeric_limits<T>::max() :
                (T(1) << N_bits) - 1;

        const size_t word_index = bit_index / BITS_PER_WORD;
        bit_index %= BITS_PER_WORD;

        if(bit_index + N_bits <= BITS_PER_WORD){
            T bits = (data[word_index] >> bit_index) & MASK;
            return bits;
        }else{
            T lower = static_cast<T>(data[word_index] >> bit_index);
            uint8_t N_lower = BITS_PER_WORD - static_cast<uint8_t>(bit_index);
            T upper = (data[word_index+1] << N_lower) & MASK;
            return upper|lower;
        }
    }

    std::string toBinaryString() const {
        std::string out;
        out.reserve(data.size()*BITS_PER_WORD - unusedBitsInLastWord());

        const size_t last_word = data.back();
        for(uint8_t i = usedBitsInLastWord(); i-->0;)
            out += bitChar(last_word, i);

        for(size_t i = data.size()-1; i-->0;)
            for(uint8_t j = BITS_PER_WORD; j-->0;)
                out += bitChar(data[i], j);

        return out;
    }

    static ByteArray fromBinaryString(std::string_view str){
        ByteArray array;
        for(size_t i = str.size(); i-->0;){
            assert(str[i] == '0' || str[i] == '1');
            array.addBit(str[i] == '1');
        }

        return array;
    }

    static ByteArray fromHexString(std::string_view str){
        ByteArray array;
        for(size_t i = str.size(); i-->0;){
            uint8_t byte = hexCharToByte(str[i]);
            array.addBits<BITS_PER_HEX_CHAR>(byte);
        }

        return array;
    }

    std::string toHexString() const {
        std::string out;
        const size_t total_bits = numBits();
        out.reserve(total_bits / BITS_PER_HEX_CHAR);

        const size_t offset = total_bits%BITS_PER_HEX_CHAR;
        size_t index = total_bits + (offset ? BITS_PER_HEX_CHAR-offset : 0);

        while(index > 0){
            index -= BITS_PER_HEX_CHAR;
            uint8_t byte = getBits<BITS_PER_HEX_CHAR, uint8_t>(index);
            out += byteToHexChar(byte);
        }

        return out;
    }

    static ByteArray fromBase64String(std::string_view str){
        ByteArray array;
        for(size_t i = str.size(); i-->0;){
            uint8_t byte = base64CharToByte(str[i]);
            array.addBits<BITS_PER_BASE64_CHAR>(byte);
        }

        return array;
    }

    std::string toBase64String() const {
        std::string out;
        const size_t total_bits = numBits();
        out.reserve(total_bits / BITS_PER_BASE64_CHAR);

        const size_t offset = total_bits%BITS_PER_BASE64_CHAR;
        size_t index = total_bits + (offset ? BITS_PER_BASE64_CHAR-offset : 0);

        while(index > 0){
            index -= BITS_PER_BASE64_CHAR;
            uint8_t byte = getBits<BITS_PER_BASE64_CHAR, uint8_t>(index);
            out += byteToBase64Char(byte);
        }

        return out;
    }

    static ByteArray exclusiveOr(const ByteArray& a, const ByteArray& b){
        assert(a.numBits() == b.numBits());

        ByteArray out;
        out.setUsedBitsInLastWord(a.usedBitsInLastWord());
        std::vector<size_t>& out_data = out.data;
        const std::vector<size_t>& a_data = a.data;
        const std::vector<size_t>& b_data = b.data;
        out_data.resize(a_data.size());

        for(size_t i = out_data.size(); i-->0;)
            out_data[i] = a_data[i] xor b_data[i];

        return out;
    }
};

}

#endif // BYTEARRAY_H
