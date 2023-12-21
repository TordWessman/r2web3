#ifndef __ETHEREUM_TRANSACTION_H__
#define __ETHEREUM_TRANSACTION_H__
#include <vector>
#include <stdint.h>

#include "../Shared/BigNumber.h"
#include "../Shared/Common.h"
#include "Transaction.h"
#include "Encodable.h"
#include "Signer.h"
#include "Transaction.h"
#include "RlpEncoder.h"
namespace blockchain
{
    /// @brief A ecdsa signature for a transaction split into it's sub-component
    struct EthereumSignature
    {
        /// @brief Creates an "unsigned signature" using chainId for the signature's `v` parameter.
        /// @param v
        EthereumSignature(uint32_t chainId) : v(chainId) {}

        /// @brief Create a signed signature using a calculated `v` and the ESCDA signature.
        /// @param v
        /// @param signature
        EthereumSignature(uint32_t v, const std::vector<uint8_t> signature)
            : v(v),
            r(signature.begin(), signature.begin() + (signature.size() / 2)),
            s(signature.begin() + (signature.size() / 2), signature.end())

        {}

        EthereumSignature(uint32_t v, const std::vector<uint8_t> r, const std::vector<uint8_t> s) : v(v), r(r), s(s) {}

        const std::vector<uint8_t> r;
        const std::vector<uint8_t> s;
        const uint32_t v;
    };

    /// @brief Signing standard being used for transactions.
    enum class EthereumSigningStandard
    {
        Legacy,
        EIP1559 // also EIP-2930
    };

    /// @brief Represents the properties required for a transaction for Ethereum compatible chains.
    struct EthereumTransactionProperties
    {
        EthereumTransactionProperties(uint32_t nonce, BigNumber gasPrice, uint32_t gasLimit,
                                      Address address, BigNumber value, std::vector<uint8_t> data,
                                      uint32_t chainId) : nonce(nonce), gasPrice(gasPrice), gasLimit(gasLimit),
                                                        address(address), value(value), data(data),
                                                        chainId(chainId), signingStandard(EthereumSigningStandard::Legacy)
        {
            
        }

        const EthereumSigningStandard signingStandard;
        const uint32_t nonce;
        const BigNumber gasPrice;
        const uint32_t gasLimit;
        const Address address;
        const BigNumber value;
        std::vector<uint8_t> data;
        const uint32_t chainId;
    };

    class EthereumTransaction : public Transaction
    {
    public:
        /// @brief Cunstruct a transaction without a signature.
        EthereumTransaction(EthereumTransactionProperties properties) : properties(properties), 
                                                                        signature(EthereumSignature(properties.chainId)),
                                                                        encoder(new RlpEncoder()) {}
        /// @brief Cunstruct a transaction including a signature.
        EthereumTransaction(EthereumTransactionProperties properties, EthereumSignature signature) : properties(properties),
                                                                                                        signature(signature),
                                                                                                        encoder(new RlpEncoder()) {}

        EthereumTransaction(const EthereumTransaction& other) : properties(other.properties), 
                                                                signature(other.signature), 
                                                                encoder(other.encoder->clone()) {}

        ~EthereumTransaction()
        {
            delete encoder;
        }

        EthereumTransaction &operator=(const EthereumTransaction &other)
        {
            if (this != &other)
            {
                delete encoder;
                encoder = other.encoder->clone();
            }
            return *this;
        }

        /// @brief Serializes the transaction using the provided `encoder`.
        /// @return 
        std::vector<uint8_t> Serialize() const override;

        EthereumTransactionProperties Properties() const { return properties; }
        //void SetEncoder(Encoder encoder) : encoder(encoder) {}

    private:
        const EthereumTransactionProperties properties;
        const EthereumSignature signature;
        EncoderX *encoder;

    };
}
#endif