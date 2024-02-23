#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <chrono>
#include <omp.h>
#include "inthash.h"
#include "fileReader.hpp"
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
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " -f <fichier1;fichier2;...> <k1> <k2> ...\n";
		return 1;
	}

	// Parsez la liste des fichiers
	std::vector<std::string> files;
	std::string fileList(argv[2]);
	std::stringstream ss(fileList);
	std::string file;
	while (std::getline(ss, file, ';')) {
		files.push_back(file);
	}

	// Parsez les tailles de k
	std::vector<int> kSizes;
	for (int i = 3; i < argc; ++i) {
		kSizes.push_back(std::stoi(argv[i]));
	}

	std::ofstream outFile("benchmark_hash_results.csv");
	outFile << "Dataset,KSize,ExecutionTime(ms),Variance\n";
	for (const auto& filename : files) {
		std::cout << filename << "..." << std::endl;
		auto sequences = FileReader::readFastaFile(filename); // Lisez une fois par fichier

		for (const auto& k : kSizes) {
			std::cout << k << "..." << std::endl;
			auto start = std::chrono::high_resolution_clock::now();
			uint64_t mask = (1ULL << (2 * k)) - 1;
			std::map<uint64_t, std::vector<uint64_t>> kmerIndex;
			auto permutation = generateRandomPermutation(k); // Générer une fois par k



#pragma omp parallel for
			for (int seqIndex = 0; seqIndex < sequences.size(); ++seqIndex) {
				auto& seq = sequences[seqIndex]; // Utilisez seqIndex pour accéder à l'élément
				auto kmers = extractKmers(seq.second, k);
				std::map<uint64_t, std::vector<uint64_t>> localIndex; // Index local pour éviter la concurrence

				for (const auto& kmer : kmers) {
					uint64_t encoded = encode(kmer);
					uint64_t hashed = hash_64(encoded, mask);
					localIndex[hashed].push_back(encoded);
				}

#pragma omp critical
				{
					for (const auto& pair : localIndex) {
						kmerIndex[pair.first].insert(kmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
					}
				}
			}



			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> elapsed = end - start;
			double variance = calculateVariance(kmerIndex);
			outFile << filename << "," << k << "," << elapsed.count() << "," << variance << "\n";
		}	
	}

	outFile.close();
	return 0;

}

