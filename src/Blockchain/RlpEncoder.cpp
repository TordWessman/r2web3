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

#include <cassert>

#include "RlpEncoder.h"
#include "../Shared/Common.h"

namespace blockchain
{
    #define RLP_LENGTH_THRESHOLD 0x37
    #define RLP_OFFSET_ARRAY_SHORT 0xc0
    #define RLP_OFFSET_ARRAY_LONG 0xf7
    #define RLP_OFFSET_ITEM_SHORT 0x80
    #define RLP_OFFSET_ITEM_LONG 0xb7
    #define RLP_MAX_LENGTH 0xff - RLP_OFFSET_ARRAY_LONG

    // Packs the length information into an RLP header
    std::vector<uint8_t> generateHeader(const size_t length)
    {
        std::vector<uint8_t> header;
        size_t tmp = length;
        while ((uint32_t)(tmp / 0x100) > 0)
        {
            header.push_back((uint8_t)(tmp % 0x100));
            tmp = (uint32_t)(tmp / 0x100);
        }
        header.push_back((uint8_t)(tmp));
        header.insert(header.begin(), RLP_OFFSET_ITEM_LONG + header.size());
        return header;
    }

    // Reverse order of the original header
    std::vector<uint8_t> reverseHeader(const std::vector<uint8_t> originalHeader)
    {
        assert(originalHeader.size() > 0);

        std::vector<uint8_t> header;
        header.push_back(originalHeader[0]);
        const uint8_t hex = originalHeader.size() - 1;
        for (int i = 0; i < hex; i++)
        {
            header.push_back(originalHeader[hex - i]);
        }
        return header;
    }

    std::vector<uint8_t> RlpEncoder::Encode(const EncodableItem *item) const
    {
        if (item->Type() == EncodableItemType::ItemArray)
        {
            assert(item->Bytes().size() == 0);
            std::vector<EncodableItem> children = item->Children();
            return EncodeVector(&children);
        }
        else
        {
            assert(item->Children().size() == 0);
            std::vector<uint8_t> bytes = item->Bytes();
            return EncodeBytes(&bytes);   
        }
    }

    std::vector<uint8_t> RlpEncoder::EncodeVector(const std::vector<EncodableItem> *items) const
    {
        std::vector<uint8_t> encoded;

        for (EncodableItem item : *items)
        {
            std::vector<uint8_t> itemBytes = Encode(&item);
            encoded.insert(encoded.end(), itemBytes.begin(), itemBytes.end());
        }

        size_t combinedCount = encoded.size();

        if (combinedCount <= RLP_LENGTH_THRESHOLD)
        {
            const uint8_t sign = RLP_OFFSET_ARRAY_SHORT + (uint8_t)combinedCount;
            encoded.insert(encoded.begin(), sign);
            return encoded;
        } 
        else 
        {
            std::vector<uint8_t> length = (encoded.size() | byte_array::size_to_bytes) | byte_array::truncate;

            assert(length.size() < RLP_MAX_LENGTH);

            const uint8_t sign = RLP_OFFSET_ARRAY_LONG + length.size();

            for (int i = 0; i < length.size(); i++) 
            {
                encoded.insert(encoded.begin(), length[length.size() - i - 1]);
            }
            encoded.insert(encoded.begin(), sign);

            return encoded;
        }
    }

    std::vector<uint8_t> RlpEncoder::EncodeBytes(const std::vector<uint8_t> *bytes) const
    {
        std::vector<uint8_t> encoded;
        const size_t length = bytes->size();

        if (length == 1 && (*bytes)[0] == 0x00)
        {
            encoded.push_back(RLP_OFFSET_ITEM_SHORT);
        }
        else if (length == 1 && (*bytes)[0] < RLP_OFFSET_ITEM_SHORT)
        {
            encoded.insert(encoded.end(), bytes->begin(), bytes->end());
        }
        else if (length <= RLP_LENGTH_THRESHOLD)
        {
            encoded.push_back((uint8_t)RLP_OFFSET_ITEM_SHORT + (uint8_t)length);
            encoded.insert(encoded.end(), bytes->begin(), bytes->end());
        }
        else
        {
            std::vector<uint8_t> header = generateHeader(length);
            header = reverseHeader(header);
            encoded.insert(encoded.end(), header.begin(), header.end());
            encoded.insert(encoded.end(), bytes->begin(), bytes->end());
        }

        return encoded;
    }
}