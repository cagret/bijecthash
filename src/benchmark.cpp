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
#include <functional>

#include "inthash.h"
#include "fileReader.hpp"

//#define DEBUG

long long getPeakMemoryUsage() {
	struct rusage rusage;
	getrusage(RUSAGE_SELF, &rusage);
	return rusage.ru_maxrss; 
}

void splitEncodedKmer(uint64_t input, uint64_t& prefix, uint64_t& suffix, size_t prefixSize) {
	uint64_t suffixMask = (1ULL << (2 * prefixSize)) - 1;
	prefix = input >> (2 * prefixSize);
	suffix = input & suffixMask;
}
long long encode(const std::string& str) {
    std::vector<size_t> charToIndex(128); // ASCII table size, but only a few indices are used
    charToIndex['A'] = 0;
    charToIndex['C'] = 1;
    charToIndex['G'] = 2;
    charToIndex['T'] = 3;

    long long encoded = 0;
    for (char c : str) {
        encoded = encoded * 4 + charToIndex[c];
    }
    return encoded;
}

std::string decode(long long encoded, size_t k) {
	std::string decoded;
	decoded.reserve(k);
	while (k--) {
		decoded += "ACGT"[encoded % 4];
		encoded /= 4;
	}
	std::reverse(decoded.begin(), decoded.end());
	return decoded;
}

std::string applyPermutation(const std::string& kmer, const std::vector<size_t>& permutation) {
	std::string permuted;
	permuted.reserve(kmer.size());
	for (size_t i : permutation) {
		permuted += kmer[i];
	}
	return permuted;
}

std::vector<size_t> generateIdentityPermutation(size_t k) {
	std::vector<size_t> p(k);
	std::iota(p.begin(), p.end(), 0);
	return p;
}

std::vector<size_t> generateInversePermutation(size_t k) {
	std::vector<size_t> p(k);
	std::iota(p.begin(), p.end(), 0); 
	std::reverse(p.begin(), p.end()); 
	return p;


}

std::vector<size_t> generateSeedSortPermutation(const std::string& sequence) {
	size_t k = sequence.length();
	std::vector<size_t> p(k);
	std::iota(p.begin(), p.end(), 0);
	unsigned long long seed = 0;
	for (char c : sequence) {
		seed += c;
	}
	std::mt19937 generator(seed); 
	std::shuffle(p.begin(), p.end(), generator);
	return p;
}

std::vector<size_t> generateCyclicPermutation(size_t k) {
	std::vector<size_t> p(k);
	std::iota(p.begin(), p.end(), 0); 
	if (k > 0) { 
		size_t temp = p.back(); 
		for (size_t i = k - 1; i > 0; --i) {
			p[i] = p[i - 1]; 
		}
		p[0] = temp; 
	}
	return p;
}

std::vector<size_t> generateZigzagPermutation(size_t k) {
	std::vector<size_t> p(k);
	size_t start = 0, end = k - 1;
	for (size_t i = 0; i < k; ++i) {
		p[i] = (i % 2 == 0) ? start++ : end--;
	}
	return p;
}

std::vector<size_t> generateRandomPermutation(size_t k) {
	std::vector<size_t> p(k);
	std::iota(p.begin(), p.end(), 0);
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(p.begin(), p.end(), g);
	return p;
}

std::vector<std::string> extractKmers(const std::string& sequence, size_t k) {
	std::vector<std::string> kmers;
	kmers.reserve(sequence.length() - k + 1);
	for (size_t i = 0; i + k <= sequence.length(); i++) {
		kmers.push_back(sequence.substr(i, k));
	}
	return kmers;
}

uint64_t rot(uint64_t s, size_t width) {
	const size_t bitSize = 64; 
	size_t halfWidth = width / 2;
	return (s << halfWidth) | (s >> (bitSize - halfWidth)); 
}

uint64_t Ga_b(uint64_t s, uint64_t a, uint64_t b, size_t sigma) {
	const uint64_t mask = (1ULL << sigma) - 1; 
	return ((a * (rot(s, sigma) ^ b)) & mask);
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


void writeData(
                const std::vector<std::vector<uint64_t>>& globalKmerIndex,
                const std::string& outputFileName) {
        std::ofstream outFile(outputFileName);
        if (!outFile.is_open()) {
                std::cerr << "Unable to open file: " << outputFileName << std::endl;
                return;
        }
        outFile << "Prefix,SuffixCount\n";
        for (size_t prefix = 0; prefix < globalKmerIndex.size(); ++prefix) {
                size_t suffixCount = globalKmerIndex[prefix].size();
                outFile << prefix << "," << suffixCount << "\n";
        }
        outFile.close();
}

void processKmersWithSpecificHashing(
		const std::vector<std::pair<std::string, std::string>>& sequences,
		size_t k, size_t prefixSize,
		size_t MAX_PREFIX,
		std::function<uint64_t(uint64_t)> hashFunction,
		const std::string& outputFileName,
		std::ofstream& outFile
		) {
	std::vector<std::vector<uint64_t>> globalKmerIndex(MAX_PREFIX);
	auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
	{
		std::vector<std::vector<uint64_t>> localKmerIndex(MAX_PREFIX);
#pragma omp for nowait
		for (size_t i = 0; i < sequences.size(); ++i) {
			const auto& seq = sequences[i];
			auto kmers = extractKmers(seq.second, k);
			for (const auto& kmer : kmers) {
				uint64_t kmerEncoded = encode(kmer); // Encodez votre k-mer ici.
				uint64_t kmerHashed = hashFunction(kmerEncoded); // Appliquez votre fonction de hachage ici.
				uint64_t prefixKmer = kmerHashed >> (64 - 2 * prefixSize); // Ajustez cette opération pour extraire le préfixe correctement.
				localKmerIndex[prefixKmer].push_back(kmerHashed); // Stockez le kmer hashé (ou une partie de celui-ci) dans le vecteur.
			}
		}
#pragma omp critical
		{
			for (size_t i = 0; i < MAX_PREFIX; ++i) {
				globalKmerIndex[i].insert(
						globalKmerIndex[i].end(),
						std::make_move_iterator(localKmerIndex[i].begin()),
						std::make_move_iterator(localKmerIndex[i].end())
						);
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	outFile << "Traitement: " << outputFileName << ", Temps écoulé: " << elapsed.count() << "ms\n";
	writeData(globalKmerIndex, outputFileName + "_data.csv");
}

void processKmersWithPermutation(const std::vector<std::pair<std::string, std::string>>& sequences, size_t k, size_t prefixSize,size_t MAX_PREFIX, const std::vector<size_t>& permutation, const std::string& outputFileName, std::ofstream& outFile) {
	std::vector<std::vector<uint64_t>> globalKmerIndex(MAX_PREFIX + 1);
	auto memoryBefore = getPeakMemoryUsage();
	auto start = std::chrono::high_resolution_clock::now();
#pragma omp parallel
	{
		std::vector<std::vector<uint64_t>> localKmerIndex(MAX_PREFIX + 1);
#pragma omp for nowait
		for (size_t i = 0; i < sequences.size(); ++i) {
			const auto& seq = sequences[i];
			auto kmers = extractKmers(seq.second, k);
			for (const auto& kmer : kmers) {
				std::string permutedKmer = applyPermutation(kmer, permutation);
				std::string prefixKmer = permutedKmer.substr(0, prefixSize);
				std::string suffixKmer = permutedKmer.substr(prefixSize);
				uint64_t prefixEncoded = encode(prefixKmer);
				uint64_t suffixEncoded = encode(suffixKmer);
				if (prefixEncoded < localKmerIndex.size()) {
					localKmerIndex[prefixEncoded].push_back(suffixEncoded);
				}
			}
		}
#pragma omp critical
		{
			for (size_t i = 0; i < localKmerIndex.size(); ++i) {
				if (!localKmerIndex[i].empty()) {
					globalKmerIndex[i].insert(globalKmerIndex[i].end(),
							std::make_move_iterator(localKmerIndex[i].begin()),
							std::make_move_iterator(localKmerIndex[i].end()));
				}
			}
		}
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//auto variance = calculateVariance(globalKmerIndex); 
	auto memoryAfter = getPeakMemoryUsage();
	auto memoryUsed = memoryAfter - memoryBefore;
	//outFile << outputFileName << "," << k << "," << elapsed.count() << "," << variance << "," << memoryUsed << "\n";
	outFile << outputFileName << "," << k << "," << elapsed.count() << "," << memoryUsed << "\n";
	writeData(globalKmerIndex, outputFileName + "_data.csv");
}

void visualizeSuffixDistribution(const std::unordered_map<uint64_t, std::vector<uint64_t>>& prefixSuffixMap, size_t k, std::ofstream& outFile){
//TODO
}

std::string determineFileType(const std::string& filename) {
	std::ifstream file(filename);
	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) { 
			if (line[0] == '>') {
				return "FASTA";
			} else if (line[0] == '@') {
				return "FASTQ";
			}
			break; 
		}
	}
	return "UNKNOWN"; 
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
	std::vector<size_t> kSizes;
	for (size_t i = 3; i < argc; ++i) {
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
			uint64_t prefixKmer, suffixKmer;
			size_t prefixSize = 11;
			size_t MAX_PREFIX = static_cast<size_t>(std::pow(4, prefixSize)); 
			long memoryBefore, memoryAfter, memoryUsed;
			uint64_t mask = (1ULL << (2 * k)) - 1;

			std::vector<std::vector<uint64_t>> globalKmerIndex;
			std::vector<size_t> identityPermutation = generateIdentityPermutation(k); 
			std::vector<size_t> randomPermutation = generateRandomPermutation(k); 
			std::vector<size_t> inversePermutation = generateInversePermutation(k);
			std::vector<size_t> cyclicPermutation = generateCyclicPermutation(k);
			std::vector<size_t> zigzagPermutation = generateZigzagPermutation(k);
			std::vector<size_t> permutation = generateSeedSortPermutation("ATCG");

			processKmersWithPermutation(sequences, k, prefixSize, MAX_PREFIX, identityPermutation, "identity", outFile);
			processKmersWithPermutation(sequences, k, prefixSize, MAX_PREFIX, randomPermutation, "random", outFile);
			processKmersWithPermutation(sequences, k, prefixSize, MAX_PREFIX, inversePermutation, "inverse", outFile);
			processKmersWithPermutation(sequences, k, prefixSize, MAX_PREFIX, cyclicPermutation, "cyclic", outFile);
			processKmersWithPermutation(sequences, k, prefixSize, MAX_PREFIX, zigzagPermutation, "zigzag", outFile);
/*
			processKmersWithSpecificHashing(sequences, k, prefixSize,MAX_PREFIX, [](uint64_t kmerEncoded) -> uint64_t {
					return Ga_b(kmerEncoded, 17, 42, k*2); // Pour GAB_HASH
					}, "GAB_Hash", outFile);

			processKmersWithSpecificHashing(sequences, k, prefixSize,MAX_PREFIX, [](uint64_t kmerEncoded) -> uint64_t {
					return hash_64(kmerEncoded, mask); // Pour IntHASH
					}, "IntHash", outFile);
*/
		}
	}
	outFile.close();
	return 0;
}

