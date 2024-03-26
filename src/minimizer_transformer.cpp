#include "minimizer_transformer.hpp"
#include <cassert>

MinimizerTransformer::MinimizerTransformer(const Settings &s) :
  Transformer(s, "Minimizer") {
    assert(settings.length-settings.prefix_length <=30);
  }

uint64_t MinimizerTransformer::xorshift(uint64_t x) const{ 
  x ^= x << 25;
  x ^= x >> 27;
  return x * 0x2545F4914F6CDD1D;
}

Transformer::EncodedKmer MinimizerTransformer::operator()(const std::string& kmer) const {
    Transformer::EncodedKmer encoded;
    uint64_t min_hash = std::numeric_limits<uint64_t>::max();
    size_t minimizer_pos = 0;
    size_t min_size = settings.prefix_length; 
    assert(kmer.size() == settings.length);

    for (size_t i = 0; i + min_size <= kmer.size(); ++i) {
        uint64_t hash = xorshift(_encode(kmer.c_str() + i, min_size));
        if (hash < min_hash) {
            min_hash = hash;
            minimizer_pos = i;
        }
    }
    assert(minimizer_pos + min_size <= kmer.size());

    size_t suffix_length = settings.length - settings.prefix_length;

    std::string before = kmer.substr(0, minimizer_pos);
    std::string minimizer = kmer.substr(minimizer_pos, min_size);
    std::string after = kmer.substr(minimizer_pos + min_size);

#ifdef DEBUG
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << " Minimizer: '" << minimizer << "', Before: '" << before << "', After: '" << after <<"'"<< std::endl;
#endif

    std::string transformed = minimizer + before + after;

    encoded.prefix = _encode(transformed.c_str(), settings.prefix_length);
    encoded.suffix = _encode(transformed.c_str() + settings.prefix_length, suffix_length);
    encoded.suffix |= minimizer_pos << (64 - 6);

    return encoded;
}


std::string MinimizerTransformer::operator()(const Transformer::EncodedKmer& encoded) const {
    size_t minimizer_pos = encoded.suffix >> (64 - 6);
    size_t min_size = settings.prefix_length;

    std::string decoded;
    decoded.reserve(settings.length);

    std::string prefix = _decode(encoded.prefix, settings.prefix_length);
    decoded += prefix;

    size_t suffix_length = settings.length - min_size - prefix.size();
    if (suffix_length > 0) {
        std::string suffix = _decode(encoded.suffix & ((1ULL << (64 - 6)) - 1), suffix_length);
        decoded += suffix;
    }

    std::string minimizer = prefix.substr(prefix.size() - min_size);
    std::string before = decoded.substr(0, minimizer_pos);
    std::string after = decoded.substr(minimizer_pos);

#ifdef DEBUG
    std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
              << " Minimizer: '" << minimizer << "', Before: '" << before << "', After: '" << after <<"'"<< std::endl;
#endif

    return before + minimizer + after;
}
