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
    /// @brief Interface for transaction generation (`TransactionType`s) using transaction information (`TransactionProperties`).
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

    /// @brief Default `TFactoryInterface` implementation, responsible for signing ethereum-compatible transactions.
    class ETFactory : TFactoryInterface<EthereumTransactionProperties, EthereumTransaction>
    {
    public:

        ETFactory() : signer(new EthereumSigner()) {}

        ~ETFactory()
        {
            delete signer;
        }

        /// @brief Creates and signs a transaction. 
        /// @param properties Transaction information.
        /// @param privateKey Key used to sign the transaction.
        /// @return A signed transaction.
        EthereumTransaction GenerateTransaction(EthereumTransactionProperties &properties, std::vector<uint8_t> *privateKey) const override
        {
            EthereumTransaction t_unsigned(properties);

            EthereumTransaction t_signed = signer->Sign(&t_unsigned, privateKey);

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

        /// @brief Return the chain id.
        /// @return 
        Result<uint32_t> GetChainId() const;

        /// @brief Return the current gas price.
        /// @return 
        Result<BigUnsigned> GetGasPrice() const;

        /// @brief Returns the balance (in gwei) for the provided `account`.
        /// @param account 
        /// @return 
        Result<BigUnsigned> GetBalance(const Account *account) const;

        /// @brief Execute a message call without creating a transaction on the block chain.
        /// @param contractCall Method exectution information
        /// @param contractAddress Contract address.
        /// @return
        Result<TransactionResponse> ViewCall(const ContractCall *contractCall, const Address &contractAddress) const;
        
        /// @brief Return the number of transactions made. Used for calculating nonce.
        /// @param account 
        /// @return 
        Result<BigUnsigned> GetTransactionCount(const Account *account) const;

        /// @brief Send a signed transaction. This could either be a transfer or a contract call.
        /// @param from Sender account
        /// @param to Receiving address
        /// @param amount Amount (gwei) to transfer. If this is a contract call, `amount` should be zero. 
        /// @param gasPrice Gas price
        /// @param gasLimit Gas limit
        /// @param contractCall Optional parameter. If provided, this `Send` invocation is considered to be a contract execution.
        /// @return The result of the transaction.
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