#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <span>
#include <sstream>
#include <vector>

#include "base64.h"
#include "bytearray.h"
#include "decrypt.h"
#include "hex.h"
#include "text_frequency_analysis.h"

using namespace CryptoFriends;

static std::string getFileContents(const char* file_name){
    std::ifstream in(file_name);
    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string str = buffer.str();
    #ifdef _WIN32
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    #endif
    return str;
}

bool Set_1_Problem_1(){
    static constexpr char bin_str[] =
        "010010010010011101101101001000000110101101101001011011000110110001101001011011100110011100100000011110010110111101110101011100100010000001100010011100100110000101101001011011100010000001101100011010010110101101100101001000000110000100100000011100000110111101101001011100110110111101101110011011110111010101110011001000000110110101110101011100110110100001110010011011110110111101101101";
    static constexpr char hex_str[] =
        "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    static constexpr char b64_str[] =
        "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
    static constexpr char ascii_str[] =
        "I'm killing your brain like a poisonous mushroom";

    struct Format {
        std::string name;
        std::string_view src;
        std::function<ByteArray(std::string_view)> builder;
        std::string (ByteArray::*printer)() const;
    };

    std::vector<Format> formats = {
        {.name="ascii",  .src=ascii_str, .builder=ByteArray::fromAscii,        .printer=&ByteArray::toAscii},
        {.name="binary", .src=bin_str,   .builder=ByteArray::fromBinaryString, .printer=&ByteArray::toBinaryString},
        {.name="hex",    .src=hex_str,   .builder=ByteArray::fromHexString,    .printer=&ByteArray::toHexString},
        {.name="base64", .src=b64_str,   .builder=ByteArray::fromBase64String, .printer=&ByteArray::toBase64String},
    };

    //Test all permutations of input and output
    bool fail = false;
    for(size_t i = 0; i < formats.size(); i++){
        for(size_t j = 0; j < formats.size(); j++){
            const Format& input = formats[i];
            const Format& output = formats[j];

            ByteArray array = input.builder(input.src);
            std::string out = (array.*output.printer)();
            bool success = (out == output.src);
            if(success) continue;
            fail = true;
            std::cout << "S1P1: failed conversion " << input.name << " => ByteArray => " << output.name << std::endl;
        }
    }

    if(!fail) std::cout << "S1P1: passing" << std::endl;

    return fail;
}

bool Set_1_Problem_2(){
    static constexpr char a_hex_str[] = "1c0111001f010100061a024b53535009181c";
    static constexpr char b_hex_str[] = "686974207468652062756c6c277320657965";
    static constexpr char xor_hex_str[] = "746865206b696420646f6e277420706c6179";

    ByteArray a = ByteArray::fromHexString(a_hex_str);
    ByteArray b = ByteArray::fromHexString(b_hex_str);
    ByteArray XOR = ByteArray::exclusiveOr(a, b);
    std::string xor_hex_eval = XOR.toHexString();

    bool fail = false;

    if(xor_hex_eval != xor_hex_str){
        fail = true;
        std::cout << "S1P2: failed XOR operation" << std::endl;
    }

    if(XOR.toAscii() != "the kid don't play"){
        fail = true;
        std::cout << "S1P2: ascii conversion failed" << std::endl;
    }

    if(!fail) std::cout << "S1P2: passing" << std::endl;

    return fail;
}

bool Set_1_Problem_3(){
    static constexpr std::string_view xor_encrypted_hex_string =
            "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736";

    static constexpr std::string_view decrypted_msg =
            "Cooking MC's like a pound of bacon";

    const ByteArray xor_encrypted_bytes = ByteArray::fromHexString(xor_encrypted_hex_string);
    bool fail = false;

    double best_score = 1;
    uint8_t best_key;
    for(uint16_t i = 0; i < 255; i++){
        ByteArray cipher;
        for(size_t j = 0; j < xor_encrypted_hex_string.length()/2; j++)
            cipher.addBits<8>(i);
        ByteArray output = ByteArray::exclusiveOr(xor_encrypted_bytes, cipher);
        std::string str_out = output.toAscii();
        toLowerCase(str_out);

        double score = getScore(str_out);
        if(score < best_score){
            best_score = score;
            best_key = static_cast<uint8_t>(i);
        }
        //std::cout << "Key " << (int)i << ": " << score << "  |  "<< str_out << std::endl;
        //88: Cooking MC's like a pound of bacon
    }

    ByteArray cipher;
    for(size_t j = 0; j < xor_encrypted_hex_string.length()/2; j++)
        cipher.addBits<8>(best_key);

    ByteArray guess = ByteArray::exclusiveOr(xor_encrypted_bytes, cipher);
    if(guess.toAscii() != decrypted_msg){
        fail = true;
        std::cout << "S1P3: failed to decrypt message" << std::endl;
    }

    if(!fail) std::cout << "S1P3: passing" << std::endl;

    return fail;
}

bool Set_1_Problem_4(){
    bool fail = false;

    static constexpr std::string_view decrypted_msg =
            "Now that the party is jumping\n";

    std::vector<std::string> lines;
    std::string line;
    std::ifstream in("4.txt");
    while(std::getline(in, line))
        if(!line.empty()) lines.push_back(line);

    double best_score = 1;
    uint8_t best_key;
    size_t best_line;
    for(size_t line_num = 0; line_num < lines.size(); line_num++){
        const std::string& line = lines[line_num];
        const ByteArray xor_encrypted_bytes = ByteArray::fromHexString(line);
        for(uint16_t i = 0; i < 255; i++){
            ByteArray cipher;
            for(size_t j = 0; j < line.length()/2; j++)
                cipher.addBits<8>(i);
            ByteArray output = ByteArray::exclusiveOr(xor_encrypted_bytes, cipher);
            std::string str_out = output.toAscii();
            toLowerCase(str_out);

            double score = getScore(str_out);
            if(score < best_score){
                best_score = score;
                best_key = static_cast<uint8_t>(i);
                best_line = line_num;
            }

            //if(score < 0.6) std::cout << (int)line_num << ", Key " << (int)i << " | " << score << " :  "<< str_out << std::endl;
            //Line 170, key 53: Now that the party is jumping
        }
    }

    line = lines[best_line];
    const ByteArray xor_encrypted_bytes = ByteArray::fromHexString(line);

    ByteArray cipher;
    for(size_t j = 0; j < line.length()/2; j++)
        cipher.addBits<8>(best_key);

    ByteArray guess = ByteArray::exclusiveOr(xor_encrypted_bytes, cipher);
    if(guess.toAscii() != decrypted_msg){
        fail = true;
        std::cout << "S1P4: failed to find/decrypt message" << std::endl;
    }

    if(!fail) std::cout << "S1P4: passing" << std::endl;

    return fail;
}

bool Set_1_Problem_5(){
    bool fail = false;

    static constexpr std::string_view plain_text =
            "Burning 'em, if you ain't quick and nimble\n"
            "I go crazy when I hear a cymbal";

    static constexpr std::string_view encrypted_hex =
            "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";

    static constexpr std::string_view key = "ICE";

    ByteArray bytes = ByteArray::fromAscii(plain_text);
    bytes.applyRepeatingKeyXor(key);
    if(bytes.toHexString() != encrypted_hex){
        fail = true;
        std::cout << "S1P5: repeated-key XOR did not produce expected result" << std::endl;
    }

    if(!fail) std::cout << "S1P5: passing" << std::endl;

    return fail;
}

bool Set_1_Problem_6(){
    bool fail = false;

    static constexpr std::string_view hamming_input_a = "this is a test";
    static constexpr std::string_view hamming_input_b = "wokka wokka!!!";

    ByteArray array_a = ByteArray::fromAscii(hamming_input_a);
    ByteArray array_b = ByteArray::fromAscii(hamming_input_b);
    size_t computed_hamming_distance = ByteArray::differingBits(array_a, array_b);
    static constexpr size_t EXPECTED_HAMMING_DISTANCE = 37;
    if(computed_hamming_distance != EXPECTED_HAMMING_DISTANCE){
        fail = true;
        std::cout << "S1P6: incorrect hamming distance" << std::endl;
    }

    std::string encryped_base64 = getFileContents("6.txt");
    encryped_base64.erase(remove(encryped_base64.begin(), encryped_base64.end(), '\n'), encryped_base64.end());
    ByteArray encrypted_bytes = ByteArray::fromBase64String(encryped_base64);

    static constexpr size_t KEY_SIZE_MIN_BYTES = 2;
    static constexpr size_t KEY_SIZE_MAX_BYTES = 40;

    struct Result{
        size_t key_size;
        double normalised_edit_distance;
    };

    std::vector<Result> results;

    for(size_t key_size = KEY_SIZE_MIN_BYTES; key_size <= KEY_SIZE_MAX_BYTES; key_size++){
        size_t differing_bits = 0;

        for(size_t byte = 0; byte < key_size; byte++){
            const size_t a = encrypted_bytes.getByte(byte);
            const size_t b = encrypted_bytes.getByte(key_size + byte);
            const size_t diff = a ^ b;
            for(uint8_t j = 0; j < BITS_PER_BYTE; j++)
                differing_bits += isBitSet(diff, j);
        }

        double normalised_edit_distance = static_cast<double>(differing_bits) / key_size;

        results.push_back(Result{
            .key_size = key_size,
            .normalised_edit_distance = normalised_edit_distance
        });
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const Result& a, const Result& b){return a.normalised_edit_distance < b.normalised_edit_distance;}
    );

    static constexpr size_t RESULTS_TO_USE = 5;
    std::span<Result> to_use(results.begin(), results.begin()+RESULTS_TO_USE);

    //DO THIS: do the rest of this problem

    for(const Result& result : to_use){
        std::vector<ByteArray> arrays;
        arrays.resize(result.key_size);

        std::vector<std::string> decoded_strings;
        decoded_strings.resize(result.key_size);

        //Divide the encryped data for single-XOR analysis
        size_t index = 0;
        for(size_t byte = 0; byte < encrypted_bytes.numBytes(); byte++){
            arrays[index++].addBits<8>( encrypted_bytes.getByte(byte) );
            if(index == arrays.size()) index = 0;
        }

        for(size_t j = 0; j < result.key_size; j++){
            const ByteArray& divided_array = arrays[j];
            double best_score = std::numeric_limits<double>::max();
            for(uint16_t i = 0; i < 255; i++){
                ByteArray cipher;
                for(size_t j = 0; j < divided_array.numBytes(); j++)
                    cipher.addBits<8>(i);
                ByteArray output = ByteArray::exclusiveOr(divided_array, cipher);
                std::string str_out = output.toAscii();
                toLowerCase(str_out);

                double score = getScore(str_out);
                if(score < best_score){
                    best_score = score;
                    decoded_strings[j] = str_out;
                }

                //std::cout << "K" << result.key_size << "i" << j << str_out << "g" << i << ": " << str_out << std::endl;
            }
        }

        std::string solved;
        solved.reserve(encrypted_bytes.numBytes());
        index = 0;
        size_t str_index = 0;
        for(size_t byte = 0; byte < encrypted_bytes.numBytes(); byte++){
            char ch = decoded_strings[index++][str_index];
            solved += ch;
            if(index == arrays.size()){
                index = 0;
                str_index++;
            }
        }

        //std::cout << "Key size " << (int)result.key_size << ":\n"
        //          << solved << '\n' << std::endl;
    }

    //Now that you probably know the KEYSIZE: break the ciphertext into blocks of KEYSIZE length.
    //Now transpose the blocks: make a block that is the first byte of every block, and a block that is the second byte of every block, and so on.
    //Solve each block as if it was single-character XOR. You already have code to do this.
    //For each block, the single-byte XOR key that produces the best looking histogram is the repeating-key XOR key byte for that block. Put them together and you have the key.

    if(!fail) std::cout << "S1P6: incomplete" << std::endl;

    return fail;
}

bool Set_1_Problem_7(){
    bool fail = false;

    static constexpr std::string_view key = "YELLOW SUBMARINE";
    std::string contents_base64 = getFileContents("7.txt");
    contents_base64.erase(remove(contents_base64.begin(), contents_base64.end(), '\n'), contents_base64.end());
    ByteArray ba = ByteArray::fromBase64String(contents_base64);
    const std::string encryped_ascii = ba.toAscii();
    const std::string plain_text = decrypt(encryped_ascii, key);

    if(plain_text != getFileContents("7_solved.txt")){
        fail = true;
        std::cout << "S1P7: failed to decrypt file with OpenSSL" << std::endl;
    }

    if(!fail) std::cout << "S1P7: passing" << std::endl;

    return fail;
}

bool Set_1_Problem_8(){
    bool fail = false;

    //EVENTUALLY: complete this
    if(!fail) std::cout << "S1P8: incomplete" << std::endl;

    return fail;
}

int main(){
    bool failed = false;
    failed |= Set_1_Problem_1();
    failed |= Set_1_Problem_2();
    failed |= Set_1_Problem_3();
    failed |= Set_1_Problem_4();
    failed |= Set_1_Problem_5();
    failed |= Set_1_Problem_6();
    failed |= Set_1_Problem_7();
    failed |= Set_1_Problem_8();

    if(!failed) std::cout << "No failures" << std::endl;

    return failed;
}
