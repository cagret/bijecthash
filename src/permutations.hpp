#ifndef PERMUTATIONS_H
#define PERMUTATIONS_H

#include <algorithm>
#include <numeric>
#include <random>
#include <string>
#include <vector>

std::string decode(long long encoded, size_t k);
std::string applyPermutation(const std::string& kmer, const std::vector<size_t>& permutation);
std::vector<size_t> generateIdentityPermutation(size_t k);
std::vector<size_t> generateInversePermutation(size_t k);
std::vector<size_t> generateSeedSortPermutation(const std::string& sequence);
std::vector<size_t> generateCyclicPermutation(size_t k);

#endif // PERMUTATIONS_H

