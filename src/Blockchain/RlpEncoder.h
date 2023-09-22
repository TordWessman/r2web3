#ifndef __RLP_ENCODER_H__
#define __RLP_ENCODER_H__

#include <vector>
#include <stdint.h>
#include "Encodable.h"

namespace blockchain
{
    /// @brief Responsible for encoding RlpItem(s) used for transactions.
    class RlpEncoder: public EncoderX
    {
    public:
        RlpEncoder() {}

        RlpEncoder* clone() const override { return new RlpEncoder(*this); }

        /// @brief RLP-encodes an item and its sub-content
        /// @param item 
        /// @return an RLP-encoded byte-array.
        std::vector<uint8_t> Encode(const EncodableItem *item) const override;

    private:
        std::vector<uint8_t> EncodeVector(const std::vector<EncodableItem> *items) const;
        std::vector<uint8_t> EncodeBytes(const std::vector<uint8_t> *bytes) const;
    };
}
#endif