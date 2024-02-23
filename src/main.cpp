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
//#define DEBUG 

long long encode(const std::string& str) {
	long long encoded = 0;
	for (char c : str) {
		int val = 0; 
		switch (c) {
			case 'A': val = 0; break; 
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


std::string applyPermutation(const std::string& kmer, const std::vector<int>& permutation) {
	std::string permuted;
	for (int i : permutation) {
		permuted += kmer[i];
	}
	return permuted;
}

std::vector<int> generateRandomPermutation(int k) {
	std::vector<int> p(k);
	std::iota(p.begin(), p.end(), 0); 
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


double calculateVariance(const std::map<uint64_t, std::vector<uint64_t>>& index) {
	double mean = 0;
	for (const auto& pair : index) {
		mean += pair.second.size();
	}
	mean /= index.size();

	double variance = 0;
	for (const auto& pair : index) {
		double diff = pair.second.size() - mean;
		variance += diff * diff;
	}
	variance /= index.size();
	return variance;
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
	int k1 = k / 3 + 2;

	uint64_t mask = (1ULL << (2 * k)) - 1;

	// Read sequences from the FASTA file
	auto sequences = FileReader::readFastaFile(filename);
	std::map<uint64_t, std::vector<uint64_t>> kmerIndex;
	// Process each sequence
	for (const auto& seq : sequences) {
#ifdef DEBUG
		std::cout << "ID: " << seq.first << std::endl;
#endif
		// Extract k-mers from the sequence
		auto kmers = extractKmers(seq.second, k);
		for (const auto& kmer : kmers) {
			uint64_t encoded = encode(kmer); 
			uint64_t hashed = hash_64(encoded, mask);
			uint64_t unhashed = hash_64i(hashed, mask);

			std::string decoded = decode(unhashed, k);

			// Print results
#ifdef DEBUG
			std::cout << "k = " << k << std::endl;
			std::cout << "K-mer original: " << kmer << std::endl;
			std::cout << "Encoded: " << encoded << std::endl;
			std::cout << "Hashed: " << hashed << ", Unhashed (encoded value): " << unhashed << std::endl;
			std::cout << "Decoded from unhashed: " << decoded << std::endl; // Ceci devrait correspondre au k-mer original si tout est correct
#endif
		}
	}

	for (const auto& seq : sequences) {
#ifdef DEBUG
		std::cout << "ID: " << seq.first << std::endl;
#endif
		// Extraction des k-mers
		auto kmers = extractKmers(seq.second, k);

		for (const auto& kmer : kmers) {
			auto permutedKmer = applyPermutation(kmer, generateRandomPermutation(kmer.size()));

			// Ajout du débogage pour afficher le k-mer permué
#ifdef DEBUG
			std::cout << "Permuted k-mer: " << permutedKmer << std::endl;
#endif
			// Calcul des parties encodées du k-mer permué
			std::string prefixKmer = permutedKmer.substr(0, k1);
			std::string suffixKmer = permutedKmer.substr(k1, k - k1);

			// Ajout du débogage pour afficher les parties encodées
#ifdef DEBUG
			std::cout << "Prefix encoded: " << prefixKmer << std::endl;
			std::cout << "Suffix encoded: " << suffixKmer << std::endl;
#endif
			uint64_t prefixEncoded = encode(prefixKmer);
			uint64_t suffixEncoded = encode(suffixKmer);
			kmerIndex[prefixEncoded].push_back(suffixEncoded);
		}
	}

	double variance = calculateVariance(kmerIndex);
	std::cout << "Variance des tailles des listes de suffixes: " << variance << std::endl;

	return 0;
}
