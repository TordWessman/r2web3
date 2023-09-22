#ifndef __ABI_ARGUMENT_ENCODER_H__
#define __ABI_ARGUMENT_ENCODER_H__

#include <vector>
#include <stdint.h>
#include "Encodable.h"

namespace blockchain
{
    /// @brief Responsible for encoding parameters for EVM calls.
    class ABIArgumentEncoder : public EncoderX
    {
    public:
        ABIArgumentEncoder() {}

        ABIArgumentEncoder *clone() const override { return new ABIArgumentEncoder(*this); }

        /// @brief Encodes an item and its sub-content into EVM compatible hex array.  
        /// @param item
        /// @return a hex-array representation of the encoded arguments.
        std::vector<uint8_t> Encode(const EncodableItem *item) const override;

    };
}
#endif