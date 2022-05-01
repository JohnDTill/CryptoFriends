#ifndef BYTEARRAY_H
#define BYTEARRAY_H

#include "base64.h"
#include "hex.h"
#include "text_frequency_analysis.h"

#include <cassert>
#include <string>
#include <vector>

namespace CryptoFriends {

template<typename T> static constexpr size_t byteSize() noexcept { return sizeof(T); }
template<typename T> static constexpr size_t bitSize() noexcept { return 8*byteSize<T>(); }
constexpr uint8_t BYTES_PER_WORD = sizeof(size_t);
constexpr uint8_t BITS_PER_BYTE = 8;
constexpr uint8_t BITS_PER_WORD = BITS_PER_BYTE*BYTES_PER_WORD;
constexpr uint8_t bitsToBytes(uint8_t bits) noexcept { return bits / BITS_PER_BYTE + (bits % BITS_PER_BYTE > 0); }
constexpr bool isBitSet(size_t word, uint8_t bit) noexcept {
    assert(bit < BITS_PER_WORD);
    return word & (size_t(1) << bit);
}
static constexpr char bitChar(size_t word, uint8_t bit) noexcept {
    return '0' + isBitSet(word, bit);
}

class ByteArray{

private:
    //Uses a vector of words to represent raw binary data
    //The vector is ordered from the least significant bits at the front to the most significant in the back
    //After an operation:
    //   The vector will have unused space. unusedBits() is never 0.

    std::vector<size_t> data = {0};
    uint8_t used_bits = 0;
    uint8_t usedBitsInLastWord() const noexcept { return used_bits; }
    uint8_t unusedBitsInLastWord() const noexcept { return BITS_PER_WORD - used_bits; }
    uint8_t usedBytesInLastWord() const noexcept { return bitsToBytes(usedBitsInLastWord()); }
    uint8_t unusedBytesInLastWord() const noexcept { return BYTES_PER_WORD - usedBytesInLastWord(); }
    void setUsedBitsInLastWord(uint8_t n_bits) noexcept {
        assert(n_bits < BITS_PER_WORD);
        used_bits = n_bits;
    }
    void setUnusedBitsInLastWord(uint8_t n_bits) noexcept {
        assert(n_bits > 0 && n_bits <= BITS_PER_WORD);
        used_bits = BITS_PER_WORD - n_bits;
    }
    size_t allocatedBits() const noexcept{ return BITS_PER_WORD*data.size(); }
    size_t allocatedBytes() const noexcept{ return BYTES_PER_WORD*data.size(); }

    uint8_t padding_front = 0; //EVENTUALLY: this is a terrible hack. Make a better design for base64.

public:
    size_t numBits() const noexcept{
        return allocatedBits() - unusedBitsInLastWord();
    }

    size_t numBytes() const noexcept{
        return allocatedBytes() - unusedBytesInLastWord();
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

    uint8_t getByte(size_t byte_index) const noexcept {
        assert(byte_index < numBytes());
        const size_t word_index = byte_index / BYTES_PER_WORD;
        const size_t shift = (byte_index % BYTES_PER_WORD) * BITS_PER_BYTE;
        return static_cast<uint8_t>(data[word_index] >> shift);
    }

    void setByte(size_t byte_index, uint8_t byte) noexcept{
        assert(byte_index < numBytes());
        const size_t word_index = byte_index / BYTES_PER_WORD;
        const size_t shift = (byte_index % BYTES_PER_WORD) * BITS_PER_BYTE;
        const size_t zero_byte_mask = ~(size_t(255) << shift);
        data[word_index] &= zero_byte_mask;
        data[word_index] ^= (static_cast<size_t>(byte) << shift);
    }

    std::string toBinaryString() const {
        std::string out;
        out.reserve(data.size()*BITS_PER_WORD - unusedBitsInLastWord());

        const size_t last_word = data.back();
        for(uint8_t i = usedBitsInLastWord(); i-->0;)
            out += bitChar(last_word, i);

        for(size_t i = data.size()-1; i-->padding_front;)
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

        while(index > padding_front){
            index -= BITS_PER_HEX_CHAR;
            uint8_t byte = getBits<BITS_PER_HEX_CHAR, uint8_t>(index);
            out += byteToHexChar(byte);
        }

        return out;
    }

    static ByteArray fromBase64String(std::string_view str){
        ByteArray array;
        for(size_t i = str.size(); i-->0;){
            if(str[i] == '='){
                array.addBits<BITS_PER_BASE64_CHAR>(0);
                array.padding_front += 8;
                continue;
            }
            else if(str[i] == '=' || str[i] == '\n' || str[i] == '\r') continue;
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

    static ByteArray fromAscii(std::string_view str){
        ByteArray array;
        for(size_t i = str.size(); i-->0;){
            uint8_t byte = str[i];
            array.addBits<BITS_PER_BYTE>(byte);
        }

        return array;
    }

    std::string toAscii() const{
        std::string out;
        const size_t total_bits = numBits();
        out.reserve(total_bits / BITS_PER_BYTE);

        const size_t offset = total_bits%BITS_PER_BYTE;
        size_t index = total_bits + (offset ? BITS_PER_BYTE-offset : 0);

        while(index > padding_front){
            index -= BITS_PER_BYTE;
            uint8_t byte = getBits<BITS_PER_BYTE, uint8_t>(index);
            out += byte;
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

    void applyRepeatingKeyXor(std::string_view keyword) noexcept {
        size_t index = 0;
        for(size_t i = numBytes(); i-->0;){
            uint8_t byte = getByte(i);
            uint8_t xor_result = byte ^ keyword[index];
            setByte(i, xor_result);
            if(++index == keyword.size()) index = 0;
        }
    }

    static size_t differingBits(const ByteArray& a, const ByteArray& b) noexcept {
        //aka Hamming distance
        size_t differing_bits = 0;
        const std::vector<size_t>& a_data = a.data;
        const std::vector<size_t>& b_data = b.data;
        for(size_t i = 0; i < a_data.size(); i++){
            const size_t diff = a_data[i] ^ b_data[i];
            for(uint8_t j = 0; j < BITS_PER_WORD; j++)
                differing_bits += isBitSet(diff, j);
        }

        return differing_bits;
    }

    double scoreGuess(size_t start, size_t offset, uint8_t guess) const noexcept {
        assert(start < offset);

        Frequency freq = {0};
        size_t total = 0;
        for(size_t i = numBytes()-start-1; i < numBytes(); i -= offset){
            freq[getByte(i) ^ guess] += 1;
            total++;
        }
        for(size_t i = 0; i < 256; i++) freq[i] /= total;

        return l1Score(freq);
    }

    uint8_t bestGuess(size_t start, size_t offset) const noexcept {
        static constexpr uint8_t LOW_GUESS = 32;
        static constexpr uint8_t HIGH_GUESS = 126;

        double best_score = scoreGuess(start, offset, LOW_GUESS);
        uint8_t best_guess = LOW_GUESS;
        for(uint8_t i = LOW_GUESS+1; i <= HIGH_GUESS; i++){
            double score = scoreGuess(start, offset, i);
            if(score < best_score){
                best_score = score;
                best_guess = i;
            }
        }
        return best_guess;
    }

    std::string bestRepeatingXorKey(size_t key_size) const {
        std::string guessed_key;
        guessed_key.resize(key_size);

        for(size_t i = 0; i < key_size; i++)
            guessed_key[i] = bestGuess(i, key_size);

        return guessed_key;
    }
};

}

#endif // BYTEARRAY_H
