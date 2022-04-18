#include <iostream>

using namespace std;

inline std::string hex2base64(const std::string& str){
    std::string out;
    out.reserve((str.size()+1)/2);

    for(size_t i = 0; i < str.size()-1; i+=2)
        out += ((static_cast<uint8_t>(str[i]) << 4) | (static_cast<uint8_t>(str[i+1])));
    if(str.size()%2 == 1) out += str.back();

    return out;
}

int main(){
    const std::string test_str = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

    cout << hex2base64(test_str) << endl;
    return 0;
}
