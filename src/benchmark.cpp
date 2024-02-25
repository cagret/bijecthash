#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <map>
#include <cmath>
#include <algorithm>
#include <random>
#include <string>
#include <chrono>
#include <numeric>
#include <omp.h>
#include <sys/resource.h>

#include "inthash.h"
#include "fileReader.hpp"

//#define DEBUG

long long getPeakMemoryUsage() {
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	return rusage.ru_maxrss; 
}


long long encode(const std::string& str) {
	static const std::unordered_map<char, int> charMap = {{'A', 0}, {'C', 1}, {'G', 2}, {'T', 3}};
	long long encoded = 0;
	for (char c : str) {
		auto it = charMap.find(c);
		if (it != charMap.end()) { // Character is known (A, C, G, T)
			encoded = encoded * 4 + it->second;
		}
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


void visualizeSuffixDistribution(const std::unordered_map<uint64_t, std::vector<uint64_t>>& prefixSuffixMap, int k, std::ofstream& outFile){
#ifdef DEBUG
	std::cout << "Prefix: " <<std::endl;
	if (!outFile.is_open()) {
		std::cerr << "Failed to open output file." << std::endl;
		return;
	}
	std::cout << "Map size: " << prefixSuffixMap.size() << std::endl;
	if (prefixSuffixMap.empty()) {
		std::cout << "Map is empty." << std::endl;
		return;
	}
#endif
	for (const auto& pair : prefixSuffixMap) {
#ifdef DEBUG
		std::cout << "Pair: " << pair.first << ", Suffixes: " << pair.second.size() << std::endl;
#endif
		const auto& prefix = pair.first;
		const auto& suffixes = pair.second;
		std::unordered_map<uint64_t, int> suffixCount; // pour compter les suffixes

		// Compter les occurrences de chaque suffixe
		for (const auto& suffix : suffixes) {
			suffixCount[suffix]++;
		}
#ifdef DEBUG
		std::cout << "Writing to file..." << std::endl;
#endif
		outFile << "Prefix: " << decode(prefix,k) << ", Suffix distribution: ";
		for (const auto& countPair : suffixCount) {
			outFile << "(" << decode(countPair.first,k) << ":" << countPair.second << ") ";
		}
		outFile << "\n";
#ifdef DEBUG
		std::cout << "Done writing to file." << std::endl;
#endif
	}
}

std::string determineFileType(const std::string& filename) {
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) { // Skip any empty lines at the start of the file
			if (line[0] == '>') {
				return "FASTA";
			} else if (line[0] == '@') {
				return "FASTQ";
			}
			break; // Exit after checking the first non-empty line
		}
	}
	return "UNKNOWN"; // Return UNKNOWN if the file does not start with expected characters
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
	if (!outFile.is_open()) {
		std::cerr << "Error: Unable to open output file.\n";
		return 1; // Exit with error code
	}
	outFile << "Dataset,KSize,ExecutionType,ExecutionTime(ms),Variance,MemoryUsed(KB)\n";
	std::vector<std::pair<std::string, std::string>> sequences;
	for (const auto& filename : files) {
		std::string fileType = determineFileType(filename);

		if (fileType == "FASTA") {
			sequences = FileReader::readFastaFile(filename);
			std::cout << "Processed " << sequences.size() << " sequences from FASTA file.\n";
		} else if (fileType == "FASTQ") {
			sequences = FileReader::readFastqFile(filename);
			std::cout << "Processed " << sequences.size() << " sequences from FASTQ file.\n";
		} else {
			std::cerr << "Error: File type unknown or file does not start with '>' or '@'.\n";
			continue; // Skip this file and continue with the next
		}
		for (const auto& k : kSizes) {
			std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
			std::chrono::duration<double, std::milli> elapsed;
			double variance;
			long memoryBefore, memoryAfter, memoryUsed;
			uint64_t mask = (1ULL << (2 * k)) - 1;

			std::unordered_map<uint64_t, std::vector<uint64_t>> globalKmerIndex;
			auto identityPermutation = generateRandomPermutation(k); // Reuse this permutation if k does not change often

			/************************    IDENTITY    ***********************************/
			globalKmerIndex.clear(); 
			memoryBefore = 0;// getPeakMemoryUsage();
			start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
			{
				std::unordered_map<uint64_t, std::vector<uint64_t>> localKmerIndex;
				//std::map<uint64_t, std::vector<uint64_t>> localKmerIndex; // Index local pour chaque thread
#pragma omp for nowait
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);
#ifdef DEBUG
					std::cout << "Extracted " << kmers.size() << " kmers from sequence " << i << std::endl;
#endif
					for (const auto& kmer : kmers) {
						std::string permutedKmer = applyPermutation(kmer, identityPermutation);
						std::string prefixKmer = permutedKmer.substr(0, k / 3 + 2);
						std::string suffixKmer = permutedKmer.substr(k / 3 + 2);

						uint64_t prefixEncoded = encode(prefixKmer);
						uint64_t suffixEncoded = encode(suffixKmer);

						localKmerIndex[prefixEncoded].push_back(suffixEncoded);
					}
				}

#pragma omp critical
				for (const auto& pair : localKmerIndex) {
#ifdef DEBUG
					std::cout << "Merging " << pair.second.size() << " suffixes for prefix " << pair.first << std::endl;
#endif
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(),
							std::make_move_iterator(pair.second.begin()),
							std::make_move_iterator(pair.second.end())); // Utiliser move pour réduire la duplication

				}
			}
#pragma omp barrier
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			memoryAfter = getPeakMemoryUsage();
			memoryUsed = memoryAfter - memoryBefore;
			outFile << filename << "," << k << ",Identity," << elapsed.count() << "," << variance << "," << memoryUsed << "\n";

			visualizeSuffixDistribution(globalKmerIndex,k,outFile);


			/************************    RANDOM    ***********************************/
			globalKmerIndex.clear(); // Nettoyage avant de commencer

			memoryBefore =0;// getPeakMemoryUsage();
			auto randomPermutation = generateRandomPermutation(k); // Génération d'une permutation aléatoire
			start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
			{
				std::unordered_map<uint64_t, std::vector<uint64_t>> localKmerIndex;
				//std::map<uint64_t, std::vector<uint64_t>> localKmerIndex; // Index local pour chaque thread

#pragma omp for nowait
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);

					for (const auto& kmer : kmers) {
						std::string permutedKmer = applyPermutation(kmer, randomPermutation);

						// Découper le k-mer permuté en préfixe et suffixe
						std::string prefixKmer = permutedKmer.substr(0, k / 3 + 2);
						std::string suffixKmer = permutedKmer.substr(k / 3 + 2);

						// Encoder le préfixe et le suffixe
						uint64_t prefixEncoded = encode(prefixKmer);
						uint64_t suffixEncoded = encode(suffixKmer);

						// Ajouter le suffixe encodé dans l'index local
						localKmerIndex[prefixEncoded].push_back(suffixEncoded);
					}
				}

#pragma omp critical
				for (const auto& pair : localKmerIndex) {
					//globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(),
							std::make_move_iterator(pair.second.begin()),
							std::make_move_iterator(pair.second.end())); // Utiliser move pour réduire la duplication
				}
			}
#pragma omp barrier
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			memoryAfter = getPeakMemoryUsage();
			memoryUsed = memoryAfter - memoryBefore;
			outFile << filename << "," << k << ",Random," << elapsed.count() << "," << variance << "," << memoryUsed << "\n";

			visualizeSuffixDistribution(globalKmerIndex,k,outFile);
			/*
			   for (const auto& pair : globalKmerIndex) {
			   const auto& prefix = pair.first;
			   const auto& suffixes = pair.second;
			   std::unordered_map<uint64_t, int> suffixCount; // pour compter les suffixes

			// Compter les occurrences de chaque suffixe
			for (const auto& suffix : suffixes) {
			suffixCount[suffix]++;
			}

			// Écrire les informations sur la répartition des suffixes
			outFile << "Prefix: " << decode(prefix) << ", Suffix distribution: ";
			for (const auto& countPair : suffixCount) {
			outFile << "(" << decode(countPair.first) << ":" << countPair.second << ") ";
			}
			outFile << "\n";
			}
			*/


			/************************    IntHASH    ***********************************/
			globalKmerIndex.clear(); // Nettoyage avant de commencer

			memoryBefore = 0;
			start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
			{
				std::unordered_map<uint64_t, std::vector<uint64_t>> localKmerIndex;
				//std::map<uint64_t, std::vector<uint64_t>> localKmerIndex; 
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
					//globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(),
							std::make_move_iterator(pair.second.begin()),
							std::make_move_iterator(pair.second.end())); // Utiliser move pour réduire la duplication
				}
			}
#pragma omp barrier
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			memoryAfter = getPeakMemoryUsage();
			memoryUsed = memoryAfter - memoryBefore;
			outFile << filename << "," << k << ",IntHash," << elapsed.count() << "," << variance << "," << memoryUsed << "\n";

			visualizeSuffixDistribution(globalKmerIndex,k,outFile);


		}
	}

	outFile.close();
	return 0;
}

