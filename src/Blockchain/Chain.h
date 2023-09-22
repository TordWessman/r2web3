#ifndef __CHAIN_H__
#define __CHAIN_H__

#include <vector>
#include <stdint.h>

#include "../Shared/Common.h"
#include "../Shared/cJSON.h"
#include "../Shared/BigUnsigned.h"
#include "../Network/NetworkFacade.h"
#include "Account.h"
#include "EthereumSigner.h"
#include "Signer.h"
#include "TransactionResponse.h"
#include "Contract.h"

namespace blockchain
{
    template <typename TransactionProperties, typename TransactionType>
    class TFactoryInterface
    {
    public:
        virtual TransactionType GenerateTransaction(TransactionProperties &properties, std::vector<uint8_t> *privateKey) const
    #ifndef ARDUINO
        {  throw std::runtime_error("<>::GenerateTransaction() must be overloaded"); }
    #else
            = 0;
    #endif
    };

    class ETFactory : TFactoryInterface<EthereumTransactionProperties, EthereumTransaction>
    {

    public:

        ETFactory() : signer(new EthereumSigner()) {}

        ~ETFactory()
        {
            delete signer;
        }

        EthereumTransaction GenerateTransaction(EthereumTransactionProperties &properties, std::vector<uint8_t> *privateKey) const override
        {
            EthereumTransaction *t_unsigned = new EthereumTransaction(properties);

            EthereumTransaction t_signed = signer->Sign(t_unsigned, privateKey);
            delete t_unsigned;

            return t_signed;
        }

        ETFactory &operator=(const ETFactory &other)
        {
            if (this != &other)
            {
                delete signer;
                signer = other.signer->clone();
            }
            return *this;
        }

    private:
        const Signer<EthereumTransaction> *signer;
    };

    /// @brief Interface to a EVM-compatible blockchain.
    class Chain
    {
    public:
        Chain(const char *rpcUrl, NetworkFacade *networkFacade) : url(rpcUrl), network(networkFacade), transactionFactory(new ETFactory()), id(0) {}
        Chain(const char *rpcUrl, NetworkFacade *networkFacade, const uint32_t chainId) : url(rpcUrl), network(networkFacade), transactionFactory(new ETFactory()), id(chainId) {}

        ~Chain() 
        {
            delete transactionFactory;
        }

        bool Start();
        bool Started() { return started; }

        Result<uint32_t> GetChainId() const;
        Result<BigUnsigned> GetGasPrice() const;
        Result<BigUnsigned> GetBalance(const Account *account) const;
        Result<TransactionResponse> ViewCall(const ContractCall *contractCall, const Address &contractAddress) const;
        Result<BigUnsigned> GetTransactionCount(const Account *account) const;
        Result<TransactionResponse> Send(const Account *from, const Address &to,
                                        const BigUnsigned &amount, const BigUnsigned &gasPrice,
                                        const uint32_t gasLimit, const ContractCall *contractCall = nullptr) const;
        

    private:
        const ETFactory *transactionFactory;
        const char *url;
        NetworkFacade *network;
        uint32_t id;
        bool started;
        void AssertStarted() const;

    };
}
#endif