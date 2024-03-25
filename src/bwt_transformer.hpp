#ifndef __BWT_TRANSFORMER_HPP__
#define __BWT_TRANSFORMER_HPP__

#include <limits>
#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include "transformer.hpp"

/**
 * Transformer that calculates a bwt from a k-mer and uses it for encoding.
 */
class BwtTransformer : public Transformer {

public:
    /**
     * Constructs a BwtTransformer with a specified bwt length.
     *
     * \param k The length of the bwt.
     */
    BwtTransformer(const Settings &settings);
    /**
     * Encodes a given k-mer into a prefix/suffix code using a bwt.
     *
     * Each derived class must overload this operator.
     *
     * \param kmer The k-mer to encode.
     *
     * \return Returns the EncodedKmer corresponding to the given k-mer.
     */
    virtual EncodedKmer operator()(const std::string &kmer) const override;
    
    /**
     * Decodes a given encoded k-mer back to its original string representation.
     *
     * Each derived class must overload this operator.
     *
     * \param e The encoded k-mer to decode.
     *
     * \return Returns the k-mer corresponding to the given encoding.
     */
    virtual std::string operator()(const EncodedKmer &e) const override;

};

#endif // __BWT_TRANSFORMER_HPP__
