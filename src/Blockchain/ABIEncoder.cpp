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

#include "ABIEncoder.h"
#include "../Shared/Common.h"

namespace blockchain
{
    #define ARGUMENT_LENGTH 32

    bool leftPad(EncodableItemType type)
    {
        return  type == EncodableItemType::Bool ||
                type == EncodableItemType::UnsignedInt ||
                type == EncodableItemType::Address;
    }

    bool rightPad(EncodableItemType type)
    {
        return type == EncodableItemType::String;
    }

    void pad(std::vector<uint8_t> *argument, const std::vector<uint8_t> &value, const bool left = true, const size_t argumentLength = ARGUMENT_LENGTH)
    {
        assert(value.size() <= argumentLength);
        size_t start = argumentLength - value.size();
        for (size_t i = 0; i < value.size(); i++)
        {
            if(left) { argument->at(start + i) = value.at(i); }
            else { argument->at(i) = value.at(i); }
        }
    }

    size_t getVectorSize(const EncodableItem *item) 
    {
        if (item->Type() == EncodableItemType::ItemArray)
        {
            return ARGUMENT_LENGTH + item->Children().size() * ARGUMENT_LENGTH;
        }
        else if (item->Type() == EncodableItemType::Binary || item->Type() == EncodableItemType::String)
        {
            return ARGUMENT_LENGTH + (item->Bytes().size() / ARGUMENT_LENGTH + 1) * ARGUMENT_LENGTH;
        }
        
        return ARGUMENT_LENGTH;
    }

    std::vector<uint8_t> ABIEncoder::Encode(const EncodableItem *item) const
    {
        std::vector<uint8_t> encoded(getVectorSize(item), 0);

        if (leftPad(item->Type()))
        {
            pad(&encoded, item->Bytes());
        } 
        else if (item->Type() == EncodableItemType::Binary || item->Type() == EncodableItemType::String)
        {
            pad(&encoded, item->Bytes().size() | byte_array::size_to_bytes);
            for(size_t i = 0; i < item->Bytes().size(); i++)
            {
                encoded[ARGUMENT_LENGTH + i] = item->Bytes()[i];
            }
        } 
        else if (item->Type() == EncodableItemType::ItemArray) 
        {
            std::vector<uint8_t> sizeAsBytes = item->Children().size() | byte_array::size_to_bytes;
            pad(&encoded, sizeAsBytes);
            size_t j;
            for (size_t i = 0; i < item->Children().size(); i++)
            {
                j = 0;
                for (const uint8_t b : Encode(&(item->Children().at(i))))
                {
                    encoded[ARGUMENT_LENGTH + j + i * ARGUMENT_LENGTH] = b;
                    j++;
                }
            }
        }
        return encoded;
    }

}