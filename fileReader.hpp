#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <string>
#include <vector>

class FileReader {
public:
    static std::vector<std::pair<std::string, std::string>> readFastaFile(const std::string& filename);
};

#endif // FILEREADER_HPP

