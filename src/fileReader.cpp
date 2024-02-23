#include "fileReader.hpp"
#include <fstream>

std::vector<std::pair<std::string, std::string>> FileReader::readFastaFile(const std::string& filename) {
    std::vector<std::pair<std::string, std::string>> sequences;

    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Erreur: Impossible d'ouvrir le fichier.");
    }

    std::string line;
    std::string id;
    std::string sequence;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        if (line[0] == '>') {
            if (!id.empty()) {
                sequences.push_back(std::make_pair(id, sequence));
                sequence.clear();
            }
            id = line.substr(1);
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

