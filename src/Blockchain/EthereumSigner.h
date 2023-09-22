#ifndef __ETHEREUM_SIGNER_H__
#define __ETHEREUM_SIGNER_H__
#include <vector>
#include <stdint.h>

#include "Transaction.h"
#include "Signer.h"
#include "EthereumTransaction.h"

namespace blockchain
{
    #define ETHEREUM_SIGNATURE_LENGTH 64
    #define KECCAK_256_LENGTH 32

    /// @brief Signing of EVM-compatible data.
    class EthereumSigner: public Signer<EthereumTransaction>
    {
    public:

        EthereumSigner() {}

        /// @brief Sign an `EthereumTransaction` using a private key and return the signed `EthereumTransaction`.    
        EthereumTransaction Sign(const EthereumTransaction *transaction, const std::vector<uint8_t> *privateKey) const override;

        EthereumSigner *clone() const override { return new EthereumSigner(*this); }

    private:
        /// @brief Sign any message using the private `key`.
        static EthereumSignature GenerateSignature(const std::vector<uint8_t> *message, const std::vector<uint8_t> *key);
    };
}
#endif
