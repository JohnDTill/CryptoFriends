#ifndef TEXTFREQUENCYANALYSIS_H
#define TEXTFREQUENCYANALYSIS_H

#include <array>
#include <numeric>
#include <string>

namespace CryptoFriends {

static constexpr size_t N_ENGLISH_LETTERS = 26;

void toLowerCase(std::string& str) noexcept {
    static constexpr char CASE_SHIFT = 'a' - 'A';
    for(size_t i = 0; i < str.size(); i++)
        if(str[i] >= 'A' && str[i] <= 'Z')
            str[i] += CASE_SHIFT;
}

static constexpr std::array<double, N_ENGLISH_LETTERS> ENGLISH_LETTER_FREQUENCIES {
    /*a*/ 0.084966,
    /*b*/ 0.020720,
    /*c*/ 0.045388,
    /*d*/ 0.033844,
    /*e*/ 0.111607,
    /*f*/ 0.018121,
    /*g*/ 0.024705,
    /*h*/ 0.030034,
    /*i*/ 0.075448,
    /*j*/ 0.001965,
    /*k*/ 0.011016,
    /*l*/ 0.054893,
    /*m*/ 0.030129,
    /*n*/ 0.066544,
    /*o*/ 0.071635,
    /*p*/ 0.031671,
    /*q*/ 0.001962,
    /*r*/ 0.075809,
    /*s*/ 0.057351,
    /*t*/ 0.069509,
    /*u*/ 0.036308,
    /*v*/ 0.010074,
    /*w*/ 0.012899,
    /*x*/ 0.002902,
    /*y*/ 0.017779,
    /*z*/ 0.002722,
};

std::array<double, N_ENGLISH_LETTERS> getFrequencies(std::string_view str) noexcept{
    std::array<double, N_ENGLISH_LETTERS> occurences = {0};
    for(char ch : str)
        if(ch >= 'a' && ch <= 'z')
            occurences[ch - 'a'] += 1;

    double total = 0;
    for(double val : occurences) total += val;
    if(total == 0) return occurences;
    for(size_t i = 0; i < N_ENGLISH_LETTERS; i++)
        occurences[i] /= total;

    return occurences;
}

double getScore(std::string_view str) noexcept {
    double sum_of_squares = 0;

    //EVENTUALLY: actually track symbols instead of cheating
    static constexpr double HIGH_PENALTY_FOR_NONPRINTABLE_CHARS = 0.5;
    static constexpr double MEDIUM_PENALTY_FOR_SYMBOLS = 0.1;
    for(char ch : str){
        uint8_t byte = ch;
        if(byte < 32 || byte > 127) sum_of_squares += HIGH_PENALTY_FOR_NONPRINTABLE_CHARS;
        else if((ch < 'a' || ch > 'z') && ch != ' ') sum_of_squares += MEDIUM_PENALTY_FOR_SYMBOLS;
        else if(ch == '\n' || ch == '\r' || ch == '\t') return 30;
    }

    auto occurences = getFrequencies(str);
    for(size_t i = 0; i < N_ENGLISH_LETTERS; i++)
        occurences[i] -= ENGLISH_LETTER_FREQUENCIES[i];

    for(double val : occurences) sum_of_squares += val*val;

    return sum_of_squares;
}

}

#endif // TEXTFREQUENCYANALYSIS_H
