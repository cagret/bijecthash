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
#include <numeric>
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


std::vector<int> generateIdentityPermutation(int k) {
	std::vector<int> identity(k);
	std::iota(identity.begin(), identity.end(), 0);
	return identity;
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

	std::ofstream outFile("benchmark_results.csv");
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::duration<double, std::milli> elapsed;
	double variance;
	outFile << "Dataset,KSize,ExecutionType,ExecutionTime(ms),Variance\n";
	for (const auto& filename : files) {
		auto sequences = FileReader::readFastaFile(filename); // Charger les séquences une fois par fichier

		// Parcourir chaque taille de k-mer
		for (const auto& k : kSizes) {
			// Déclaration préalable pour éviter les redéclarations
			std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
			std::chrono::duration<double, std::milli> elapsed;
			double variance;

			// Utilisation d'une map globale pour stocker les résultats finaux
			std::map<uint64_t, std::vector<uint64_t>> globalKmerIndex;

			// Test avec la permutation identité
			std::vector<int> identityPermutation = generateIdentityPermutation(k);

			start = std::chrono::high_resolution_clock::now();

			// Initialisation de la parallélisation avec OpenMP
#pragma omp parallel shared(globalKmerIndex)
			{
				std::map<uint64_t, std::vector<uint64_t>> localKmerIndex; // Index local pour chaque thread

#pragma omp for nowait // Distribuer les itérations de la boucle entre les threads sans attendre
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);

					for (const auto& kmer : kmers) {
						std::string permutedKmer = applyPermutation(kmer, identityPermutation);
						std::string prefixKmer = permutedKmer.substr(0, k / 3 + 2);
						std::string suffixKmer = permutedKmer.substr(k / 3 + 2);

						uint64_t prefixEncoded = encode(prefixKmer);
						uint64_t suffixEncoded = encode(suffixKmer);

						localKmerIndex[prefixEncoded].push_back(suffixEncoded);
					}
				}

				// Fusionner les index locaux dans l'index global de manière sécurisée
#pragma omp critical
				for (const auto& pair : localKmerIndex) {
					globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
				}
			}

			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;
			variance = calculateVariance(globalKmerIndex);
			outFile << filename << "," << k << ",Identity," << elapsed.count() << "," << variance << "\n";
			/***********************************************************/
			globalKmerIndex.clear();

			// Générer une permutation aléatoire pour cette exécution
			auto randomPermutation = generateRandomPermutation(k);

			// Marquer le début du traitement
			start = std::chrono::high_resolution_clock::now();

			// Utilisation d'OpenMP pour paralléliser la boucle sur les séquences
#pragma omp parallel shared(globalKmerIndex)
			{
				std::map<uint64_t, std::vector<uint64_t>> localKmerIndex; // Index local pour chaque thread

				// Parallélisation de la boucle sur les séquences
#pragma omp for nowait // Distribuer les itérations de la boucle entre les threads sans attendre
				for (size_t i = 0; i < sequences.size(); ++i) {
					const auto& seq = sequences[i];
					auto kmers = extractKmers(seq.second, k);

					// Boucle sur chaque k-mer extrait
					for (const auto& kmer : kmers) {
						// Appliquer la permutation aléatoire au k-mer courant
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

				// Fusionner les index locaux dans l'index global de manière sécurisée
#pragma omp critical
				{
					for (const auto& pair : localKmerIndex) {
						globalKmerIndex[pair.first].insert(globalKmerIndex[pair.first].end(), pair.second.begin(), pair.second.end());
					}
				}
			}

			// Calculer le temps d'exécution après le traitement
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;

			// Calculer la variance des tailles des tables pour cette permutation
			variance = calculateVariance(globalKmerIndex);

			// Enregistrer les résultats pour la permutation aléatoire dans le fichier de sortie
			outFile << filename << "," << k << ",Random," << elapsed.count() << "," << variance << "\n";

		}
	}

	outFile.close();
	return 0;
}
