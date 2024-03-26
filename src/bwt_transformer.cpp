#include "bwt_transformer.hpp"
#include <cassert>


BwtTransformer::BwtTransformer(const Settings& s) : Transformer(s, "Bwt") {
    assert(settings.length - settings.prefix_length <= 30);
}

Transformer::EncodedKmer BwtTransformer::operator()(const std::string& kmer) const {
    EncodedKmer encoded;
    int n = kmer.length();
    size_t bwt_pos = 0;
    std::vector<std::string> rotations(n);
    
    for (int i = 0; i < n; ++i) {
        int shift = (i + bwt_pos) % n;
        rotations[i] = kmer.substr(shift) + kmer.substr(0, shift);
    }

    std::sort(rotations.begin(), rotations.end());
    int newEndPos = 0;
    std::string result;
    for (int i = 0; i < n; ++i) {
        result += rotations[i][n - 1];
        if (rotations[i].substr(n - bwt_pos - 1) == kmer.substr(0, bwt_pos + 1) &&
            rotations[i].substr(0, n - bwt_pos - 1) == kmer.substr(bwt_pos + 1)) {
            newEndPos = i;
        }
    }
    bwt_pos = newEndPos;
    assert(bwt_pos < 64); // lyndon_pos can be encoded in only 6 bits
    std::string bwt_rotation;
    bwt_rotation.reserve(kmer.size());
    bwt_rotation = kmer.substr(bwt_pos) + kmer.substr(0, bwt_pos);

    encoded.prefix = _encode(bwt_rotation.c_str(), settings.prefix_length);
    encoded.suffix = _encode(bwt_rotation.c_str()+settings.prefix_length, settings.length - settings.prefix_length);
    encoded.suffix |= bwt_pos << 58;

    return encoded;
}


std::string BwtTransformer::operator()(const Transformer::EncodedKmer& e) const {
    size_t bwt_pos = (e.suffix >> 58);

    std::string prefix = _decode(e.prefix, settings.prefix_length);
    std::string suffix = _decode(e.suffix, settings.length - settings.prefix_length);
    std::string bwt_rotation = prefix + suffix;
    std::string bwt = bwt_rotation.substr(bwt_rotation.size() - bwt_pos) + bwt_rotation.substr(0, bwt_rotation.size() - bwt_pos);

    int n = bwt.length();
    std::vector<int> T(n);
    std::map<char, std::vector<int>> charPositions;

    for (int i = 0; i < n; ++i) {
        charPositions[bwt[i]].push_back(i);
    }

    int index = 0;
    for (auto& p : charPositions) {
        for (int pos : p.second) {
            T[index++] = pos;
        }
    }

    std::string original(n, ' ');
    int next = T[bwt_pos]; 
    for (int i = n - 1; i >= 0; --i) {
        original[i] = bwt[next];
        next = T[next];
    }

    return original;
}
