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
  size_t min_size = settings.prefix_length;
  uint64_t min_hash = std::numeric_limits<uint64_t>::max();
  size_t minimizer_pos = 0;

#ifdef DEBUG
  std::cerr << "Processing kmer: " << kmer << std::endl;
#endif

  for (size_t i = 0; i + min_size <= kmer.size(); ++i) {
    uint64_t hash = xorshift(_encode(kmer.c_str() + i, min_size));
    if (hash < min_hash) {
      min_hash = hash;
      minimizer_pos = i;
    }
  }
  assert(minimizer_pos < 64); //minimizer_pos can only be encoded in 6 bits
#ifdef DEBUG
  std::cerr << "Minimizer position: " << minimizer_pos << std::endl;
#endif
  std::string before = kmer.substr(0, minimizer_pos);
  std::string minimizer = kmer.substr(minimizer_pos, min_size);
  std::string after = kmer.substr(minimizer_pos + min_size);

  std::string transformed;
  transformed.reserve(kmer.size());
  transformed = minimizer + before + after;
  /*
     std::string prefix = transformed.substr(0, minimizer_pos);
     std::string suffix = transformed.substr(minimizer_pos);
     encoded.prefix = _encode(prefix.c_str(), settings.prefix_length);
     encoded.suffix = _encode(suffix.c_str()+settings.length,settings.length - settings.prefix_length);
     */
  encoded.prefix = _encode(transformed.c_str(), settings.prefix_length);
  encoded.suffix = _encode(transformed.c_str()+settings.length,settings.length - settings.prefix_length);
  encoded.suffix |= minimizer_pos << 58;


  return encoded;
  }


  std::string MinimizerTransformer::operator()(const Transformer::EncodedKmer& e) const {
    size_t minimizer_pos = e.suffix >> 58;
    size_t min_size = settings.prefix_length;

    std::string minimizer = _decode(e.prefix, settings.prefix_length) + _decode(e.suffix, settings.length - settings.prefix_length);
    std::string kmer = minimizer.substr(minimizer.size() - minimizer_pos) + minimizer.substr(0, minimizer.size() - minimizer_pos);
#ifdef DEBUG
    std::cerr << "Debug: Encoded Kmer: " << e.prefix << " " << e.suffix << std::endl;
    std::cerr << "Debug: Kmer: " << kmer << std::endl;
    std::cerr << "Debug: Minimizer: " << minimizer << std::endl;
    std::cerr << "Debug: Minimizer position: " << minimizer_pos << std::endl;
#endif

    return kmer;
    /*
       std::string before = kmer.substr(min_size, minimizer_pos);
       std::string after = kmer.substr(minimizer_pos + min_size);
       std::string unsplit = before + minimizer + after;

       return unsplit;
       */
  }
