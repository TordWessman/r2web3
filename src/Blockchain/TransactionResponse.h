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

#ifndef __TRANSACTION_RESPONSE_H__
#define __TRANSACTION_RESPONSE_H__

#include <cstring>
#include <vector>

#include "../Shared/cJSON.h"
#include "../Shared/BigNumber.h"
#include "../Shared/Common.h"
#include "Address.h"

namespace blockchain
{
    
    #define ETH_HASH_SIZE 64

    /// @brief Arbitrary response container for requests. Will contain the `"result"` value for an ethereum request.
    class TransactionResponse
    {

    public:
        /// @brief Empty response.
        TransactionResponse() : result(nullptr) {}

        /// @brief Default constructor. `result` must be a null-terminated string.
        TransactionResponse(char *result) : result(result) {}

        TransactionResponse(const TransactionResponse &other)
        {
            if (other.result != nullptr)
            {
                result = other.result | char_string::retain;
            }
            else
            {
                result = nullptr;
            }
        }

        ~TransactionResponse()
        {
            if (result != nullptr)
            {
                delete[] result;
            }
            result = nullptr;
        }

        TransactionResponse *clone() const { return new TransactionResponse(*this); }

        TransactionResponse &operator=(const TransactionResponse &other)
        {
            if (this != &other)
            {
                if (other.result != nullptr)
                {
                    result = other.result | char_string::retain;
                }
                else
                {
                    result = nullptr;
                }
            }
            return *this;
        }

        /// @brief Returns the (internally managed) `"result"` data.
        /// @return
        char *Result() { return result; }

    private:
        char *result;
    };

    /// @brief Transaction information
    struct TransactionReceipt
    {
        TransactionReceipt() {}

        TransactionReceipt(cJSON *result) : blockNumber(cJSON_GetObjectItemCaseSensitive(result, "blockNumber")->valuestring),
                                            cumulativeGasUsed(cJSON_GetObjectItemCaseSensitive(result, "cumulativeGasUsed")->valuestring),
                                            gasUsed(cJSON_GetObjectItemCaseSensitive(result, "gasUsed")->valuestring),
                                            from(cJSON_GetObjectItemCaseSensitive(result, "from")->valuestring),
                                            to(cJSON_GetObjectItemCaseSensitive(result, "to")->valuestring)
        {
            char *bhash = cJSON_GetObjectItemCaseSensitive(result, "blockHash")->valuestring;
            strncpy(blockHash, bhash, ETH_HASH_SIZE + 2);
            blockHash[ETH_HASH_SIZE + 2] = '\0';
            char *thash = cJSON_GetObjectItemCaseSensitive(result, "transactionHash")->valuestring;
            strncpy(transactionHash, thash, ETH_HASH_SIZE + 2);
            transactionHash[ETH_HASH_SIZE + 2] = '\0';
        }

        char blockHash[ETH_HASH_SIZE + 2 + 1]; //Fit the leading `0x` and the trailing null-termination character.
        char transactionHash[ETH_HASH_SIZE + 2 + 1];
        BigNumber blockNumber;
        BigNumber cumulativeGasUsed;
        BigNumber gasUsed;
        Address from;
        Address to;

        #define TransactionReceipt_Keys "blockNumber", "cumulativeGasUsed", "gasUsed", "from", "to", "transactionHash"

        static Result<TransactionReceipt *> Parse(cJSON *result)
        {

            for (const char *key : {TransactionReceipt_Keys}) 
            {
                if (!cJSON_HasObjectItem(result, key)) 
                {
                    return Result<TransactionReceipt *>::Err(-40, key);
                }
            }
            return Result<TransactionReceipt *>(new TransactionReceipt(result));
        }
    };
    
    /// @brief Information about a mined block.
    struct BlockInformation
    {

        BlockInformation(cJSON *result) : timestamp(BigNumber(cJSON_GetObjectItemCaseSensitive(result, "timestamp")->valuestring).ToUInt32()) {}
        uint32_t timestamp;

        #define BlockInformation_Keys "timestamp"
        static Result<BlockInformation *> Parse(cJSON *result)
        {

            for (const char *key : {BlockInformation_Keys})
            {
                if (!cJSON_HasObjectItem(result, key))
                {
                    return Result<BlockInformation *>::Err(-40, key);
                }
            }
            return Result<BlockInformation *>(new BlockInformation(result));
        }
    };
}
#endif