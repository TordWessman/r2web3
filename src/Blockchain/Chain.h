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

#ifndef __CHAIN_H__
#define __CHAIN_H__

#include <vector>
#include <stdint.h>

#include "../Shared/Common.h"
#include "../Shared/cJSON.h"
#include "../Shared/BigNumber.h"
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

        /// @brief Returns the estimated gas price for a transaction
        /// @param from
        /// @param to
        /// @param amount
        /// @param gasPrice
        /// @param gasLimit
        /// @param contractCall a `ContractCall` object for an RPC invocation or `nullptr` for a transaction. 
        /// @return
        Result<BigNumber> EstimateGas(const Account *from, const Address *to,
                                      const BigNumber *amount, const BigNumber *gasPrice,
                                      const uint32_t gasLimit, const ContractCall *contractCall) const;

        /// @brief Return the current gas price.
        /// @return 
        Result<BigNumber> GetGasPrice() const;

        /// @brief Returns the balance (in gwei) for the provided `account`.
        /// @param account 
        /// @return
        Result<BigNumber> GetBalance(const Address *address) const;

        /// @brief Returns the balance of an ERC20-contract address
        /// @param address 
        /// @param contractAddress 
        /// @return 
        Result<BigNumber> GetBalance(const Address *address, const Address *contractAddress) const;

        /// @brief Execute a message call without creating a transaction on the block chain.
        /// @param contractCall Method exectution information
        /// @param contractAddress Address of the caller.
        /// @param contractAddress Contract address.
        /// @return
        Result<TransactionResponse> ViewCall(const Address *callerAddress, const Address *contractAddress, const ContractCall *contractCall) const;

        /// @brief Return the number of transactions made. Used for calculating nonce.
        /// @param account 
        /// @return
        Result<BigNumber> GetTransactionCount(const Address *address) const;

        /// @brief Send a signed transaction. This could either be a transfer or a contract call.
        /// @param from Sender account
        /// @param to Receiving address
        /// @param amount Amount (gwei) to transfer. If this is a contract call, `amount` should be zero.
        /// @param gasLimit Gas limit
        /// @param gasPrice Optional parameter. Gas price. If not specified, `GetGasPrice()` will be used.
        /// @param contractCall Optional parameter. If provided, this `Send` invocation is considered to be a contract execution.
        /// @return The result of the transaction.
        Result<TransactionResponse> Send(const Account *from, const Address *to,
                                        const BigNumber *amount, const uint32_t gasLimit,
                                        const BigNumber *gasPrice = nullptr, const ContractCall *contractCall = nullptr) const;

        /// @brief Returns a transaction `TransactionReceipt` for the specified transaction or `null` if no transaction was found.
        /// @param transactionHash
        /// @return
        Result<TransactionReceipt*> GetTransactionReceipt(const char *transactionHash) const;

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