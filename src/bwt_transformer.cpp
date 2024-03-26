#include "bwt_transformer.hpp"
#include <cassert>


BwtTransformer::BwtTransformer(const Settings& s) : Transformer(s, "Bwt") {
    assert(settings.length - settings.prefix_length <= 30);
}

Transformer::EncodedKmer BwtTransformer::operator()(const std::string& kmer) const {
    EncodedKmer encoded;
    int n = kmer.length();
    size_t bwt_pos =0;
    int pos = n - 1;
    std::vector<std::string> rotations(n);
    
    for (int i = 0; i < n; ++i) {
        int shift = (i + pos) % n;
        rotations[i] = kmer.substr(shift) + kmer.substr(0, shift);
    }

    std::sort(rotations.begin(), rotations.end());
    std::string result;
    for (int i = 0; i < n; ++i) {
        result += rotations[i][n - 1];
        if (rotations[i].substr(n - pos - 1) == kmer.substr(0, pos + 1) &&
            rotations[i].substr(0, n - pos - 1) == kmer.substr(pos + 1)) {
            bwt_pos = i;
        }
    }
    
#ifdef DEBUG
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " kmer: '" << kmer << "'" 
       << " BWT rotation index: '" << bwt_pos << "'" << std::endl;
#endif

    assert(bwt_pos < 64); // lyndon_pos can be encoded in only 6 bits
    std::string bwt_rotation;
    bwt_rotation.reserve(kmer.size());
    bwt_rotation = kmer.substr(bwt_pos) + kmer.substr(0, bwt_pos);
    assert(bwt_rotation.size() == kmer.size());

#ifdef DEBUG
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
       << " BWT rotation: '" << bwt_rotation << "'" << std::endl;
#endif

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
    
    return bwt;
}
