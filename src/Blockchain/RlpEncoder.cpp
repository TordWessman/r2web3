#include <cassert>

#include "RlpEncoder.h"
#include "../Shared/Common.h"

namespace blockchain
{
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
        std::vector<uint8_t> result;
        for (EncodableItem item : *items)
        {
            std::vector<uint8_t> itemBytes = Encode(&item);
            result.insert(result.end(), itemBytes.begin(), itemBytes.end());
        }

        size_t combinedCount = result.size();

        if (combinedCount <= 55) 
        {
            uint8_t sign = 0xC0 + (uint8_t)combinedCount;
            result.insert(result.begin(), sign);
            return result;
        } 
        else 
        {
            std::vector<uint8_t> length = (result.size() | byte_array::size_to_bytes) | byte_array::truncate;

            if (length.size() > 0xFF - 0xF7)
            {
                THROW("lengthCount to high.");
            }

            uint8_t sign = 0xF7 + length.size();

            for (int i = 0; i < length.size(); i++) 
            {
                result.insert(result.begin(), length[length.size() - i - 1]);
            }
            result.insert(result.begin(), sign);

            return result;
        }
    }

    std::vector<uint8_t> RlpEncoder::EncodeBytes(const std::vector<uint8_t> *bytes) const
    {
        
    ///TODO: Rewrite this
        std::vector<uint8_t> result;
        uint16_t inputLength = bytes->size();
        if (inputLength == 1 && (*bytes)[0] == 0x00)
        {
            result.push_back(0x80);
        }
        else if (inputLength == 1 && (*bytes)[0] < 128)
        {
            result.insert(result.end(), bytes->begin(), bytes->end());
        }
        else if (inputLength <= 55)
        {
            uint8_t _ = (uint8_t)0x80 + (uint8_t)inputLength;
            result.push_back(_);
            result.insert(result.end(), bytes->begin(), bytes->end());
        }
        else
        {
            std::vector<uint8_t> tmp_header;
            uint32_t tmp = inputLength;
            while ((uint32_t)(tmp / 256) > 0)
            {
                tmp_header.push_back((uint8_t)(tmp % 256));
                tmp = (uint32_t)(tmp / 256);
            }
            tmp_header.push_back((uint8_t)(tmp));
            uint8_t len = tmp_header.size(); // + 1;
            tmp_header.insert(tmp_header.begin(), 0xb7 + len);

            // fix direction for header
            std::vector<uint8_t> header;
            header.push_back(tmp_header[0]);
            uint8_t hexdigit = tmp_header.size() - 1;
            for (int i = 0; i < hexdigit; i++)
            {
                header.push_back(tmp_header[hexdigit - i]);
            }

            result.insert(result.end(), header.begin(), header.end());
            result.insert(result.end(), bytes->begin(), bytes->end());
        }
        return result;

    }
}