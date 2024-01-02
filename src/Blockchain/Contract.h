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

#ifndef __CONTRACT_H__
#define __CONTRACT_H__

#include "ABIEncoder.h"
#include <vector>
#include <stdint.h>

namespace blockchain
{
    #define KECCAK256_SIGNATURE_SIZE 4

    /// @brief A `ContractCall` is an object that represents a smart contract invocation and this object is passed as a parameter to 
    /// a `Chain.Send` or `Chain.ViewCall`.
    class ContractCall 
    {
    public:
        /// @brief Call a contract method without parameters.
        /// @param functionName The name of the function (not the full signature).
        ContractCall(const char *functionName) : functionName(functionName), arguments({})
        {
            GenerateSignatureHash();
        }

        /// @brief Function call using parameters.
        /// @param functionName The name of the function (not the full signature).
        /// @param arguments A `std::vector` of arguments to pass. Each argument is represented as an `EncodableItem` which
        ///                  contains the data and the signature description of that particular argument.
        ///                  The invocation can be simplified using the `ENC` macro.
        ///                  Example: `ContractCall("foo", {ENC(10u, "uint256"), ENC("bar", "string")})` 
        ///                  will have the signature `foo(uint256,string)`.
        ///
        ContractCall(const char *functionName, const std::vector<EncodableItem> arguments) : 
                functionName(functionName), 
                arguments(arguments) 
        {
            GenerateSignatureHash();
        }

        std::vector<uint8_t> AsData() const;

    private:
        const char *functionName;
        const std::vector<EncodableItem> arguments;
        std::vector<uint8_t> signatureHash;
        void GenerateSignatureHash();
        ABIEncoder encoder;
    };

    // class Contract
    // {

    //     public:

    //     Contract(Chain* contractChain, std::string contractAddress) 
    //     {
    //         chain = contractChain;
    //         address = contractAddress;
    //     }

    //     private:

    //     Chain *chain;
    //     std::string address;
    // };
}

#endif