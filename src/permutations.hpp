#ifndef PERMUTATIONS_H
#define PERMUTATIONS_H

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <vector>

void splitEncodedKmer(uint64_t input, uint64_t& prefix, uint64_t& suffix, size_t prefixSize);
long long encode(const std::string& str);
std::string decode(long long encoded, size_t k);
std::string applyPermutation(const std::string& kmer, const std::vector<size_t>& permutation);
std::vector<size_t> generateIdentityPermutation(size_t k);
std::vector<size_t> generateInversePermutation(size_t k);
std::vector<size_t> generateSeedSortPermutation(const std::string& sequence);
std::vector<size_t> generateCyclicPermutation(size_t k);
std::vector<size_t> generateZigzagPermutation(size_t k);
std::vector<size_t> generateRandomPermutation(size_t k);

uint64_t rot(uint64_t s, size_t width);
uint64_t Ga_b(uint64_t s, uint64_t a, uint64_t b, size_t sigma);


#endif // PERMUTATIONS_H

