#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

#include "base64.h"
#include "bytearray.h"
#include "hex.h"

using namespace CryptoFriends;

bool Set_1_Problem_1(){
    static constexpr char bin_str[] =
        "010010010010011101101101001000000110101101101001011011000110110001101001011011100110011100100000011110010110111101110101011100100010000001100010011100100110000101101001011011100010000001101100011010010110101101100101001000000110000100100000011100000110111101101001011100110110111101101110011011110111010101110011001000000110110101110101011100110110100001110010011011110110111101101101";
    static constexpr char hex_str[] =
        "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    static constexpr char b64_str[] =
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";

    struct Format {
        std::string name;
        std::string_view src;
        std::function<ByteArray(std::string_view)> builder;
        std::string (ByteArray::*printer)() const;
    };

    std::vector<Format> formats = {
        {.name="binary", .src=bin_str, .builder=ByteArray::fromBinaryString, .printer=&ByteArray::toBinaryString},
        {.name="hex",    .src=hex_str, .builder=ByteArray::fromHexString,    .printer=&ByteArray::toHexString},
        {.name="base64", .src=b64_str, .builder=ByteArray::fromBase64String, .printer=&ByteArray::toBase64String},
    };

    //Test all permutations of input and output
    bool failed = false;
    for(size_t i = 0; i < formats.size(); i++){
        for(size_t j = 0; j < formats.size(); j++){
            const Format& input = formats[i];
            const Format& output = formats[j];

            ByteArray array = input.builder(input.src);
            std::string out = (array.*output.printer)();
            bool success = (out == output.src);
            if(success) continue;
            failed = true;
            std::cout << "S1P1: failed conversion " << input.name << " => ByteArray => " << output.name << std::endl;
        }
    }

    return failed;
}

bool Set_1_Problem_2(){
    static constexpr char a_hex_str[] = "1c0111001f010100061a024b53535009181c";
    static constexpr char b_hex_str[] = "686974207468652062756c6c277320657965";
    static constexpr char xor_hex_str[] = "746865206b696420646f6e277420706c6179";

    ByteArray a = ByteArray::fromHexString(a_hex_str);
    ByteArray b = ByteArray::fromHexString(b_hex_str);
    ByteArray XOR = ByteArray::exclusiveOr(a, b);
    std::string xor_hex_eval = XOR.toHexString();

    bool failed = (xor_hex_eval != xor_hex_str);
    if(failed) std::cout << "S1P2: failed XOR operation" << std::endl;

    return failed;
}

int main(){
    bool failed = false;
    failed |= Set_1_Problem_1();
    failed |= Set_1_Problem_2();

    if(!failed) std::cout << "Tests passing" << std::endl;

    return failed;
}
