#ifndef TEXTFREQUENCYANALYSIS_H
#define TEXTFREQUENCYANALYSIS_H

#include <array>
#include <cassert>
#include <numeric>
#include <string>

#include <frequency_table.h>

namespace CryptoFriends {

static constexpr size_t PERMUTATIONS_PER_BYTE = 256;

std::array<double, PERMUTATIONS_PER_BYTE> getFrequencies(std::string_view str) noexcept {
    assert(!str.empty());
    std::array<double, PERMUTATIONS_PER_BYTE> occurences = {0};
    for(uint8_t byte : str) occurences[byte] += 1;
    double total = str.size();
    for(size_t i = 0; i < PERMUTATIONS_PER_BYTE; i++) occurences[i] /= total;

    return occurences;
}

double l1Score(std::string_view str) noexcept {
    double residual = 0;
    auto frequencies = getFrequencies(str);
    for(size_t i = 0; i < PERMUTATIONS_PER_BYTE; i++)
        residual += std::abs(frequencies[i] - FREQUENCY_MAP[i]);

    return residual;
}

double l2Score(std::string_view str) noexcept {
    double residual = 0;
    auto frequencies = getFrequencies(str);
    for(size_t i = 0; i < PERMUTATIONS_PER_BYTE; i++)
        residual += pow(frequencies[i] - FREQUENCY_MAP[i], 2);

    return residual;
}

}

#endif // TEXTFREQUENCYANALYSIS_H
