#include "bwt_transformer.hpp"
#include <cassert>


BwtTransformer::BwtTransformer(const Settings& s) : Transformer(s, "Bwt") {
    assert(settings.length - settings.prefix_length <= 30);
}

Transformer::EncodedKmer BwtTransformer::operator()(const std::string& kmer) const {
    EncodedKmer encoded;
    std::string text = kmer + "$";
    size_t bwt_pos = 0;

    for (size_t i = 1; i < text.size(); ++i) {
        if (text.substr(i) + text.substr(0, i) < text.substr(bwt_pos) + text.substr(0, bwt_pos)) {
            bwt_pos = i;
        }
    }

    std::string bwt_rotation = text.substr(bwt_pos) + text.substr(0, bwt_pos);
    std::string prefix = bwt_rotation.substr(0, settings.prefix_length);
    std::string suffix = bwt_rotation.substr(settings.prefix_length);

    encoded.prefix = _encode(prefix.c_str(), settings.prefix_length);
    encoded.suffix = _encode(suffix.c_str(), settings.length - settings.prefix_length);
    encoded.suffix |= bwt_pos << 58;

    return encoded;
}

std::string BwtTransformer::operator()(const Transformer::EncodedKmer& e) const {
    size_t bwt_pos = (e.suffix >> 58);
    std::string prefix = _decode(e.prefix, settings.prefix_length);
    std::string suffix = _decode(e.suffix, settings.length - settings.prefix_length);
    std::string bwt_rotation = prefix + suffix;
    std::string bwt = bwt_rotation.substr(bwt_rotation.size() - 1) + bwt_rotation.substr(0, bwt_rotation.size() - 1);

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
    int next = T[bwt_pos]; // Position du caractère de fin '$'
    for (int i = n - 1; i >= 0; --i) {
        original[i] = bwt[next];
        next = T[next];
    }

    return original.substr(0, original.size() - 1);
}
