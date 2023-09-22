#include "EthereumTransaction.h"

namespace blockchain
{
    std::vector<uint8_t> EthereumTransaction::Serialize() const
    {
        EncodableItem data({EncodableItem(properties.nonce),
                            EncodableItem(&properties.gasPrice),
                            EncodableItem(properties.gasLimit),
                            EncodableItem(&properties.address),
                            EncodableItem(&properties.value),
                            EncodableItem(properties.data), 
                            EncodableItem(signature.v), 
                            EncodableItem(signature.r), 
                            EncodableItem(signature.s)});

        std::vector<uint8_t> encoded = encoder->Encode(&data);

        if(properties.signingStandard == EthereumSigningStandard::EIP1559) 
        {
            encoded.insert(encoded.begin(), 0x02);
        }
        return encoded;
    }
}