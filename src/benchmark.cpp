#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <random>
#include <string>
#include <chrono>
#include <numeric>
#include <omp.h>

#include "inthash.h"
#include "fileReader.hpp"

long long encode(const std::string& str) {
	static const std::unordered_map<char, int> charMap = {{'A', 0}, {'C', 1}, {'G', 2}, {'T', 3}};
	long long encoded = 0;
	for (char c : str) {
		encoded = encoded * 4 + charMap.at(c);
	}
	return encoded;
}

std::string decode(long long encoded, int k) {
	std::string decoded;
	decoded.reserve(k);
	while (k--) {
		decoded += "ACGT"[encoded % 4];
		encoded /= 4;
	}
	std::reverse(decoded.begin(), decoded.end());
	return decoded;
}

std::string applyPermutation(const std::string& kmer, const std::vector<int>& permutation) {
	std::string permuted;
	permuted.reserve(kmer.size());
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
	kmers.reserve(sequence.length() - k + 1);
	for (size_t i = 0; i + k <= sequence.length(); i++) {
		kmers.push_back(sequence.substr(i, k));
	}
	return kmers;
}

double calculateVariance(const std::unordered_map<uint64_t, std::vector<uint64_t>>& index) {
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

	std::vector<std::string> files;
	std::string fileList(argv[2]);
	std::stringstream ss(fileList);
	std::string file;
	while (std::getline(ss, file, ';')) {
		files.push_back(file);
	}

	std::vector<int> kSizes;
	for (int i = 3; i < argc; ++i) {
		kSizes.push_back(std::stoi(argv[i]));
	}

	std::ofstream outFile("benchmark2_results.csv");
	outFile << "Dataset,KSize,ExecutionType,ExecutionTime(ms),Variance\n";

	for (const auto& filename : files) {
		auto sequences = FileReader::readFastaFile(filename);

		for (const auto& k : kSizes) {
			std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
			std::chrono::duration<double, std::milli> elapsed;
			double variance;
			uint64_t mask = (1ULL << (2 * k)) - 1;

			std::unordered_map<uint64_t, std::vector<uint64_t>> globalKmerIndex;
			auto identityPermutation = generateRandomPermutation(k); // Reuse this permutation if k does not change often

			/************************    IDENTITY    ***********************************/
			start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
			{
				std::unordered_map<uint64_t, std::vector<uint64_t>> localKmerIndex;

#pragma omp for nowait
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);

					for (const auto& kmer : kmers) {
						uint64_t encoded = encode(kmer);
						localKmerIndex[encoded].push_back(encoded);
					}
				}

#pragma omp critical
				for (const auto& pair : localKmerIndex) {
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
				}
			}
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			outFile << filename << "," << k << ",Identity," << elapsed.count() << "," << variance << "\n";



			/************************    RANDOM    ***********************************/
			globalKmerIndex.clear(); // Nettoyage avant de commencer
			auto randomPermutation = generateRandomPermutation(k); // Génération d'une permutation aléatoire

			start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
			{
				std::unordered_map<uint64_t, std::vector<uint64_t>> localKmerIndex;

#pragma omp for nowait
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);

					for (const auto& kmer : kmers) {
						std::string permutedKmer = applyPermutation(kmer, randomPermutation);
						uint64_t encoded = encode(permutedKmer);
						localKmerIndex[encoded].push_back(encoded);
					}
				}

#pragma omp critical
				for (const auto& pair : localKmerIndex) {
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
				}
			}
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			outFile << filename << "," << k << ",Random," << elapsed.count() << "," << variance << "\n";

			/************************    IntHASH    ***********************************/
			globalKmerIndex.clear(); // Nettoyage avant de commencer

			start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
			{
				std::unordered_map<uint64_t, std::vector<uint64_t>> localKmerIndex;

#pragma omp for nowait
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);

					for (const auto& kmer : kmers) {
						uint64_t encoded = encode(kmer);
						uint64_t hashed = hash_64(encoded, mask); // Utilisation de la fonction de hachage IntHash
						localKmerIndex[hashed].push_back(encoded);
					}
				}

#pragma omp critical
				for (const auto& pair : localKmerIndex) {
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
				}
			}
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			outFile << filename << "," << k << ",IntHash," << elapsed.count() << "," << variance << "\n";

		}
	}

	outFile.close();
	return 0;
}
