/**
 * MIT License
 *
 * Copyright (c) 2023 Tord Wessman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __ABI_ENCODER_H__
#define __ABI_ENCODER_H__

#include <vector>
#include <stdint.h>
#include "Encodable.h"

namespace blockchain
{
    /// @brief Responsible for encoding parameters for EVM calls.
    class ABIEncoder : public EncoderX
    {
    public:
        ABIEncoder() {}

        ABIEncoder *clone() const override { return new ABIEncoder(*this); }

        /// @brief Encodes an item and its sub-content into EVM compatible hex array.  
        /// @param item
        /// @return a hex-array representation of the encoded arguments.
        std::vector<uint8_t> Encode(const EncodableItem *item) const override;

    };
}
#endif