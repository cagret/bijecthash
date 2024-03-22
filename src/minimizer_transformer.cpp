#include "minimizer_transformer.hpp"

MinimizerTransformer::MinimizerTransformer(const Settings &s) :
        Transformer(s, "Minimizer"),
        m(s.length)
{}


uint64_t MinimizerTransformer::xorshift(const std::string &s, size_t start, size_t end) const {
    uint64_t hash = 0;
    for (size_t i = start; i < end; ++i) {
        hash ^= static_cast<uint64_t>(s[i]);
        hash ^= hash << 13;
        hash ^= hash >> 7;
        hash ^= hash << 17;

        #ifdef DEBUG
            std::cerr << "[^ &&& DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                      << " start: " << start << ", end: " << end << ", i: " << i
                      << ", s[i]: " << s[i] << ", hash: '" << hash <<"'"<< std::endl;
        #endif
    }
    return hash;
}



std::string MinimizerTransformer::minimizer(const std::string &s) const {
        #ifdef DEBUG
            std::cerr << "[-DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                      << " s.size():" << s.size() << ", m: " << m << std::endl;
        #endif
    if (s.size() < m) return s;

    uint64_t min_hash = std::numeric_limits<uint64_t>::max();
        #ifdef DEBUG
            std::cerr << "[-DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                      << " min_hash = " << min_hash << std::endl;
        #endif
    size_t min_start = 0;

    for (size_t i = 0; i <= s.size() - m; ++i) {
        uint64_t hash = xorshift(s, i, i + m);
        if (hash < min_hash) {
            min_hash = hash;
            min_start = i;
        }
    }
    std::string before = s.substr(0, min_start);
    std::string minimizer = s.substr(min_start, m);
    std::string after = s.substr(min_start + m);
 #ifdef DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Minimizer: " << minimizer << ", Before: " << before << ", After: " << after << std::endl;
    #endif
    return minimizer + before + after;
}


std::string MinimizerTransformer::minimizer_unsplit(const std::string& s) const {
    std::string s_copy = s;
    size_t m = 0;
    while (std::isdigit(s_copy.back())) {
        m = m * 10 + (s_copy.back() - '0');
        s_copy.pop_back();
    }
    std::reverse(s_copy.begin() + m, s_copy.end() - m);
    std::string minimizer = s_copy.substr(0, m);
    std::string before = s_copy.substr(m, s_copy.size() - 2 * m);
    std::string after = s_copy.substr(s_copy.size() - m);
 #ifdef DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Minimizer: " << minimizer << ", Before: " << before << ", After: " << after << std::endl;
    #endif
    return before + minimizer + after;
}

Transformer::EncodedKmer MinimizerTransformer::operator()(const std::string &kmer) const {
    Transformer::EncodedKmer encoded;
    std::string minStr = minimizer(kmer);
        #ifdef DEBUG
            std::cerr << "[-DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                      << " minStr:" << minStr << std::endl;
        #endif
    if (minStr == kmer) {
	encoded.prefix = xorshift(kmer, 0, settings.prefix_length);
        encoded.suffix = xorshift(kmer, settings.prefix_length, kmer.length());
        
        return encoded;
    }
    size_t min_start = kmer.find(minStr);

    std::string prefix = kmer.substr(0, min_start);
    std::string suffix = kmer.substr(min_start + minStr.length());

    encoded.prefix = xorshift(prefix, 0, prefix.length());
    encoded.suffix = xorshift(suffix, 0, suffix.length());

    #ifdef DEBUG
        std::cerr << "[#########DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Minimizer: " << minStr << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Prefix: " << prefix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Suffix: " << suffix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Encoded prefix: " << encoded.prefix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Encoded suffix: " << encoded.suffix << std::endl;
    #endif

    return encoded;
}

std::string MinimizerTransformer::operator()(const Transformer::EncodedKmer &e) const {
    //if (e.prefix == 0 && e.suffix == 0) {
    //    return _decode(0, settings.length);
    //}
    std::string prefix = _decode(e.prefix, settings.prefix_length);
    std::string suffix = _decode(e.suffix, settings.length - settings.prefix_length);
    std::string kmer = prefix + suffix;

    #ifdef DEBUG
        std::cerr << "[______DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Encoded prefix: " << e.prefix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Encoded suffix: " << e.suffix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Decoded prefix: " << prefix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Decoded suffix: " << suffix << std::endl;
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Decoded kmer: " << kmer << std::endl;
    #endif

    std::string unsplit = minimizer_unsplit(kmer);

    #ifdef DEBUG
        std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
                  << " Unsplit kmer: " << unsplit << std::endl;
    #endif
    return unsplit;
}

