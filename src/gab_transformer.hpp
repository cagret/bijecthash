#ifndef GAB_TRANSFORMER_HPP
#define GAB_TRANSFORMER_HPP

#include <transformer.hpp>

class GaBTransformer : public Transformer {
private:
    uint64_t _a; 
    uint64_t _b; 
    size_t _sigma; 
    const uint64_t _prefix_shift;
    const uint64_t _suffix_mask;
public:

    /**
   * Builds a Transformer depending on the k-mer length and the prefix
   * length.
   *
   * \param k The k-mer length.
   *
   * \param prefix_length The k-mer prefix length.
   */
    GaBTransformer(size_t k, size_t prefix_length, uint64_t a, uint64_t b, size_t sigma);
    /**
   * Encode some given k-mer into a prefix/suffix code.
   *
   * Each derived class must overload this operator.
   *
   * \param kmer The k-mer to encode.
   *
   * \return Returns the EncodedKmer corresponding to the given k-mer.
   */
    virtual Transformer::EncodedKmer operator()(const std::string &kmer) const override;
    /**
   * Decode some given encoded k-mer.
   *
   * Each derived class must overload this operator.
   *
   * \param e The encoded k-mer to decode.
   *
   * \return Returns the k-mer corresponding to the given encoding.
   */
    virtual std::string operator()(const Transformer::EncodedKmer &e) const override;
    
    static uint64_t rot(uint64_t s, size_t width);
    static uint64_t Ga_b(uint64_t s, uint64_t a, uint64_t b, size_t sigma);
};

#endif 

