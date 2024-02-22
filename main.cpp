#include "fileReader.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>
#include <string>
#include <unordered_map>

long long encode(const std::string& str) {
	long long encoded = 0;
	for (char c : str) {
		encoded = encoded * 4 + (c - 'A');
	}
	return encoded;
}


std::string decode(long long encoded, int k) {
	std::string decoded;
	while (k--) {
		decoded += "ACGT"[encoded % 4];
		encoded /= 4;
	}
	std::reverse(decoded.begin(), decoded.end());
	return decoded;
}


std::string permute(const std::string& s, const std::vector<int>& p) {
	std::string permuted(s.size(), 'A');
	for (size_t i = 0; i < s.size(); ++i) {
		permuted[i] = s[p[i]];
	}
	return permuted;
}


std::vector<int> generateRandomPermutation(int k) {
	std::vector<int> p(k);
	std::iota(p.begin(), p.end(), 0); // Remplit p avec 0, 1, ..., k-1
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(p.begin(), p.end(), g);
	return p;
}


std::vector<std::string> extractKmers(const std::string& sequence, int k) {
	std::vector<std::string> kmers;
	for (size_t i = 0; i + k <= sequence.length(); i++) {
		kmers.push_back(sequence.substr(i, k));
	}
	return kmers;
}


std::string permuteKmers(const std::vector<std::string>& kmers, const std::vector<int>& permutation) {
	std::vector<std::string> permutedKmers(kmers.size());
	for (size_t i = 0; i < kmers.size(); i++) {
		permutedKmers[i] = kmers[permutation[i]];
	}

	std::string permutedSequence;
	for (const auto& kmer : permutedKmers) {
		permutedSequence += kmer;
	}
	return permutedSequence;
}



int main(int argc, char* argv[]) {
	if (argc != 4 || std::string(argv[1]) != "-f") {
		std::cerr << "Usage: " << argv[0] << " -f <filename> <k>" << std::endl;
		return 1;
	}

	std::string filename = argv[2];
	int k = std::stoi(argv[3]); // Taille des k-mers

	auto sequences = FileReader::readFastaFile(filename);
	for (const auto& seq : sequences) {
		std::cout << "ID: " << seq.first << std::endl;
		std::cout << "Sequence: " << seq.second << std::endl;
		auto kmers = extractKmers(seq.second, k);

		for (const auto& kmer : kmers) {
			// Génère une permutation aléatoire pour chaque k-mer
			std::vector<int> permutation = generateRandomPermutation(kmer.size());
			std::string permutedK = permute(kmer, permutation);
			std::cout << "K-mer original: " << kmer << std::endl;
			std::cout << "K-mer permuted: " << permutedK << std::endl;
		}
	}

	return 0;
}


