#include "minimizer_transformer.hpp"
#include <cassert>
#include "minimizer_transformer.hpp"

MinimizerTransformer::MinimizerTransformer(const Settings &s) :
  Transformer(s, "Minimizer") {
  assert(settings.length-settings.prefix_length <=30);
}

uint64_t MinimizerTransformer::xorshift(uint64_t x) const{ 
  x ^= x << 25;
  x ^= x >> 27;
  return x * 0x2545F4914F6CDD1D;
}

std::string MinimizerTransformer::minimizer(std::string const &s, size_t &minimizer_pos)const {
  size_t min_size = settings.prefix_length;
  uint64_t min_hash = std::numeric_limits<uint64_t>::max();

  for (size_t i = 0; i + min_size <= s.size() ; ++i) {
    uint64_t hash = xorshift(_encode(s.c_str() + i, min_size));
    if (hash < min_hash) {
      min_hash = hash;
      minimizer_pos = i;
    }
  }
  std::string before = s.substr(0, minimizer_pos);
  std::string minimizer = s.substr(minimizer_pos, min_size);
  std::string after = s.substr(minimizer_pos + min_size);

  return minimizer + before + after;
}


std::string MinimizerTransformer::minimizer_unsplit(std::string& s, size_t pos) const {
  size_t min_size = settings.prefix_length;
#ifdef DEBUG
  std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
    << "Input string: " << s
    << " pos: " << pos << ", settings.length: " << settings.length << ", s.length(): " << s.length() << std::endl;
#endif
  std::string minimizer = s.substr(0, min_size);
  std::string before = s.substr(min_size, pos);
  std::string after = s.substr(pos +min_size);
#ifdef DEBUG
  std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
    << " Minimizer: " << minimizer << ", Before: " << before << ", After: " << after << std::endl;
#endif
  return before + minimizer + after;
}

Transformer::EncodedKmer MinimizerTransformer::operator()(const std::string &kmer) const {
  Transformer::EncodedKmer encoded;
  size_t minimizer_pos=0;
  std::string transformed = minimizer(kmer,minimizer_pos);
  std::string prefix = transformed.substr(0,minimizer_pos);
  std::string suffix = transformed.substr(minimizer_pos);

  encoded.prefix = _encode(prefix.c_str(), prefix.length());
  encoded.suffix = _encode(suffix.c_str(), suffix.length());
  encoded.suffix |= minimizer_pos<<58;

  return encoded;
}

std::string MinimizerTransformer::operator()(const Transformer::EncodedKmer &e) const {
  size_t minimizer_pos = (e.suffix >> 58);
  std::string kmer;
  std::string prefix = _decode(e.prefix, minimizer_pos);
  std::string suffix = _decode(e.suffix, settings.length-settings.prefix_length-minimizer_pos);
  kmer = prefix + suffix;

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

  std::string unsplit = minimizer_unsplit(kmer,minimizer_pos);

#ifdef DEBUG
  std::cerr << "[DEBUG] input encoded kmer - prefix: '" << e.prefix <<"',suffix: '" << e.suffix <<"'"<< std::endl;
  std::cerr << "[DEBUG] decoded kmer: '" << kmer <<"'"<< std::endl;
  std::cerr << "[DEBUG] unsplit kmer: '" << unsplit << "'"<< std::endl;
#endif

  return unsplit;
}     

