#include "minimizer_transformer.hpp"

MinimizerTransformer::MinimizerTransformer(const Settings &s) :
	Transformer(s, "Minimizer")
{}

std::string MinimizerTransformer::minimizer(const std::string &s) const {
	size_t k = settings.length;
	uint64_t min_hash = std::numeric_limits<uint64_t>::max();
	size_t min_start = 0;
#ifdef DEBUG
	std::cerr << "[-----------------------------------DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
		<< " k (settings.length : '" << k << "', s.size(): '" << s.size() << "', min_hash :'" << min_hash <<"'" << std::endl;
#endif

	for (size_t i = 0; i <= s.size() - k; ++i) {
		uint64_t hash = _encode(s.c_str() + i, k);
		if (hash < min_hash) {
			min_hash = hash;
			min_start = i;
		}
	}
	std::string before = s.substr(0, min_start);
	std::string minimizer = s.substr(min_start, k);
	std::string after = s.substr(min_start + k);
#ifdef DEBUG
	std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
		<< " Minimizer: " << minimizer << ", Before: " << before << ", After: " << after << std::endl;
#endif
	return minimizer + before + after;
}


std::string MinimizerTransformer::minimizer_unsplit(std::string& s, size_t pos) const {
#ifdef DEBUG
	std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
		<< "Input string: " << s
		<< " pos: " << pos << ", settings.length: " << settings.length << ", s.length(): " << s.length() << std::endl;
#endif
	std::string minimizer = s.substr(0, settings.length);
	std::string before = s.substr(settings.length, pos);
	std::string after = s.substr(pos + settings.length);
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
	size_t min_start = kmer.find(minStr);

	if (min_start == 0 && minStr.length() == kmer.length()) {
		encoded.prefix = _encode(kmer.c_str(), kmer.length());
		encoded.suffix = 0;
		encoded.pos = 0;
#ifdef DEBUG
		std::cerr << "[#########DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
			<< " Minimizer: " << minStr << std::endl;
		std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
			<< " Prefix: " << kmer << std::endl;
		std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
			<< " Suffix: " << "" << std::endl;
		std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
			<< " Encoded prefix: " << encoded.prefix << std::endl;
		std::cerr << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ":"
			<< " Encoded suffix: " << encoded.suffix << std::endl;
#endif
	} else {
		std::string prefix = kmer.substr(0, min_start);
		std::string suffix = kmer.substr(min_start + minStr.length());

		encoded.prefix = _encode(prefix.c_str(), prefix.length());
		encoded.suffix = _encode(suffix.c_str(), suffix.length());

		if (min_start != std::string::npos && min_start < kmer.length()) {
			encoded.pos = min_start;
		} else {
			throw std::runtime_error("Invalid minimizer position");
		}

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
	}

	return encoded;
}

std::string MinimizerTransformer::operator()(const Transformer::EncodedKmer &e) const {
	std::string kmer;
	std::string prefix = _decode(e.prefix, e.pos);
	std::string suffix = _decode(e.suffix, settings.length - e.pos - settings.length);
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

	std::string unsplit = minimizer_unsplit(kmer,e.pos);

#ifdef DEBUG
	std::cerr << "[DEBUG] input encoded kmer - prefix: '" << e.prefix <<"',suffix: '" << e.suffix <<"'"<< std::endl;
	std::cerr << "[DEBUG] decoded kmer: '" << kmer <<"'"<< std::endl;
	std::cerr << "[DEBUG] unsplit kmer: '" << unsplit << "'"<< std::endl;
#endif

	return unsplit;
}	    
