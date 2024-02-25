#include "fileReader.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

std::vector<std::pair<std::string, std::string>>  FileReader::readFastaFile(const std::string& filename) {
	std::vector<std::pair<std::string, std::string>> sequences;
	std::ifstream file(filename);

	if (!file.is_open()) {
		throw std::runtime_error("Error: Cannot open file.");
	}

	std::string line, id, sequence;
	while (std::getline(file, line)) {
		if (line.empty()) continue;

		if (line[0] == '>') {
			if (!id.empty()) {
				sequences.push_back(std::make_pair(id, sequence));
				sequence.clear();
			}
			id = line.substr(1); // Remove '>' and save ID
		} else {
			sequence += line;
		}
	}

	if (!id.empty()) {
		sequences.push_back(std::make_pair(id, sequence));
	}

	file.close();
	return sequences;
}


std::vector<std::pair<std::string, std::string>>  FileReader::readFastqFile(const std::string& filename) {
	std::vector<std::pair<std::string, std::string>> sequences;
	std::ifstream file(filename);

	if (!file.is_open()) {
		throw std::runtime_error("Error: Cannot open file.");
	}

	std::string line, id, seq, plus, qual;
	while (std::getline(file, id)) {
		if (id.empty() || id[0] != '@') continue; // Skip empty lines or malformed headers

		if (!std::getline(file, seq) || !std::getline(file, plus) || !std::getline(file, qual)) {
			throw std::runtime_error("Error: Unexpected end of file or malformed FASTQ record.");
		}

		sequences.push_back(std::make_pair(id.substr(1), seq)); // Use id.substr(1) to remove '@'
	}

	file.close();
	return sequences;
}
