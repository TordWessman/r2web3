#ifndef __CONTRACT_H__
#define __CONTRACT_H__

#include "ABIEncoder.h"
#include <vector>
#include <stdint.h>

namespace blockchain
{
    #define KECCAK256_SIGNATURE_SIZE 4

    class ContractCall 
    {
    public:
        ContractCall(const char *functionName) : functionName(functionName), arguments({})
        {
            GenerateSignatureHash();
        }
        
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