#include "minimizer_transformer.hpp"
#include <limits>
#include <string>

MinimizerTransformer::MinimizerTransformer(const Settings &s) : 
	Transformer(s, "Minimizer"), 
	m(m) 
{}

uint64_t MinimizerTransformer::xorshift(const std::string &s, size_t start, size_t end) const {
    uint64_t hash = 0;
    for (size_t i = start; i < end; ++i) {
        hash ^= static_cast<uint64_t>(s[i]);
        hash ^= hash << 13;
        hash ^= hash >> 7;
        hash ^= hash << 17;
    }
    return hash;
}

std::string MinimizerTransformer::minimizer(const std::string &s) const {
    if (s.size() < m) return s;

    uint64_t min_hash = std::numeric_limits<uint64_t>::max();
    size_t min_start = 0;

    for (size_t i = 0; i <= s.size() - m; ++i) {
        uint64_t hash = xorshift(s, i, i + m);
        if (hash < min_hash) {
            min_hash = hash;
            min_start = i;
        }
    }

    return s.substr(min_start, m);
}

Transformer::EncodedKmer MinimizerTransformer::operator()(const std::string &kmer) const {
    Transformer::EncodedKmer encoded;
    std::string minStr = minimizer(kmer);
    encoded.prefix = xorshift(minStr, 0, minStr.length());
    encoded.suffix = xorshift(kmer, 0, kmer.length()); 

    return encoded;
}

std::string MinimizerTransformer::operator()(const Transformer::EncodedKmer &e) const {
    return "Decoding not implemented";
}
