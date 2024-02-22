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
#include "inthash.h"

long long encode(const std::string& str) {
	long long encoded = 0;
	for (char c : str) {
		int val = 0;
		switch (c) {
			case 'C': val = 1; break;
			case 'G': val = 2; break;
			case 'T': val = 3; break;
		}
		encoded = encoded * 4 + val;
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
	// Check if the correct number of arguments are provided
	if (argc != 4 || std::string(argv[1]) != "-f") {
		std::cerr << "Error: Invalid arguments. Usage: " << argv[0] << " -f <filename> <k>" << std::endl;
		return 1;
	}

	// Parse command-line arguments
	std::string filename = argv[2];
	int k = std::stoi(argv[3]); // Size of k-mers

	// Validate k
	if (k <= 0) {
		std::cerr << "Error: Invalid value for k. Please provide a positive integer." << std::endl;
		return 1;
	}

	uint64_t mask = (1ULL << k) - 1;

	// Read sequences from the FASTA file
	auto sequences = FileReader::readFastaFile(filename);

	// Process each sequence
	for (const auto& seq : sequences) {
		std::cout << "ID: " << seq.first << std::endl;

		// Extract k-mers from the sequence
		auto kmers = extractKmers(seq.second, k);
		for (const auto& kmer : kmers) {
			uint64_t encoded = encode(kmer); // Assurez-vous que encode retourne un uint64_t

			uint64_t hashed = hash_64(encoded, mask);
			uint64_t unhashed = hash_64i(hashed, mask);

			std::string decoded = decode(unhashed, k);

			// Print results
			std::cout << "K-mer original: " << kmer << std::endl;
			std::cout << "Encoded: " << encoded << std::endl;
			std::cout << "Hashed: " << hashed << ", Unhashed (encoded value): " << unhashed << std::endl;
			std::cout << "Decoded from unhashed: " << decoded << std::endl; // Ceci devrait correspondre au k-mer original si tout est correct

		}
	}

	uint64_t key = 572581857647311335; // Encoded value
	uint64_t maske = (1ULL << 60) - 1; // Juste un exemple de mask, ajustez selon la taille de vos données

	uint64_t hashed = hash_64(key, maske);
	uint64_t unhashed = hash_64i(hashed, maske);

	std::cout << "Key: " << key << ", Hashed: " << hashed << ", Unhashed: " << unhashed << std::endl;

	// Ceci devrait afficher que "Key" et "Unhashed" sont identiques si tout est correct.

	return 0;
}
