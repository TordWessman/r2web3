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

#include "Contract.h"
#include "Signer.h"

#include "../Shared/Common.h"

namespace blockchain
{
    #define ARGUMENT_LENGTH 32

    void ContractCall::GenerateSignatureHash()
    {
        size_t signature_size = 2 + strlen(functionName);

        for (auto argument : arguments)
        {
            signature_size += 1 + strlen(argument.Handle());
        }
        if (arguments.size() > 0)
        {
            signature_size -= 1;
        }

        std::vector<uint8_t> signatureVector(signature_size);
        std::vector<uint8_t> functionNameVector = functionName | byte_array::string_to_bytes;
        size_t i = 0;
        for(uint8_t b : (functionName | byte_array::string_to_bytes))
        {
            signatureVector[i] = b;
            i++;
        }
        signatureVector[i] = '(';

        i++;

        for (size_t j = 0; j < arguments.size(); j++)
        {
            for (uint8_t b : arguments[j].Handle() | byte_array::string_to_bytes)
            {
                signatureVector[i] = b;
                i++;
            }

            if (j < arguments.size() - 1) 
            { 
                signatureVector[i] = ',';
                i++; 
            }
        }

        signatureVector[i] = ')';
        std::vector<uint8_t> hash = Keccak256(&signatureVector, KECCAK256_SIGNATURE_SIZE);
        signatureHash.insert(signatureHash.begin(), hash.begin(), hash.end());
    }

    void padx(std::vector<uint8_t> *argument, const std::vector<uint8_t> &value, const bool left = true, const size_t argumentLength = ARGUMENT_LENGTH)
    {
        assert(value.size() <= argumentLength);
        size_t start = argumentLength - value.size();
        for (size_t i = 0; i < value.size(); i++)
        {
            if (left) { argument->at(start - i) = value.at(value.size() - i - 1); }
            else { argument->at(i) = value.at(i); }
        }
    }

    bool isDynamic(const EncodableItem *argument)
    {
        return argument->Type() == EncodableItemType::ItemArray ||
               argument->Type() == EncodableItemType::Binary ||
               argument->Type() == EncodableItemType::String;
    }

    std::vector<uint8_t> ContractCall::AsData() const {

        std::vector<uint8_t> callData = signatureHash;
        std::vector<std::vector<uint8_t>> params;
        size_t dataPosition = 0;//callData.size();
        for(size_t i = 0; i < arguments.size(); i++)
        {
            params.push_back(encoder.Encode(&arguments[i]));
            if (isDynamic(&arguments[i]))
            {
                dataPosition += ARGUMENT_LENGTH;
            }
            else
            {
                dataPosition += params[i].size();
            }
        }

        size_t currentPosition = 0;
        
        for (size_t i = 0; i < arguments.size(); i++)
        {
            if (isDynamic(&arguments[i]))
            {
                std::vector<uint8_t> positionVector(ARGUMENT_LENGTH, 0);
                padx(&positionVector, dataPosition | byte_array::size_to_bytes);
                callData.insert(callData.end(), positionVector.begin(), positionVector.end());
                dataPosition += params[i].size();
            } 
            else
            {
                callData.insert(callData.end(), params[i].begin(), params[i].end());
            }
        }

        for (size_t i = 0; i < arguments.size(); i++)
        {
            if (isDynamic(&arguments[i]))
            {
                callData.insert(callData.end(), params[i].begin(), params[i].end());
            }
        }

        return callData;
    }
}