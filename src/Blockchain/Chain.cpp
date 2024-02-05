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

#include "Chain.h"
#include "../Network/HttpRequest.h"
#include "../Shared/R2Web3Log.h"
#include "Internal/Chain_ethRequest.h"

namespace blockchain
{
    bool Chain::Start()
    {
        if (id == 0)
        {
            Result<uint32_t> chainIdResult = LoadChainId();
            if (chainIdResult.HasValue())
            {
                id = chainIdResult.Value();
            }
            else
            {
                Log::e("Unable to fetch chainId: ", chainIdResult.ErrorMessage());
                return false;
            }
        }
        started = true;
        return true;
    }

    Result<uint32_t> Chain::LoadChainId() const
    {
        Result<char *> result = MakeRequst("eth_chainId", {}, false);

        if (result.HasValue())
        {
            uint32_t value = strtol(result.Value(), nullptr, 16);
            delete []result.Value();
            return Result<uint32_t>(value);
        }

        return Result<uint32_t>::Err(result);
    }

    Result<BigNumber> Chain::GetBalance(const Address address, const Address contractAddress) const
    {
        ContractCall getBalanceCall("balanceOf", {ENC(address)});
        Result<TransactionResponse> result = ViewCall(address, contractAddress, &getBalanceCall);
        if (result.HasValue())
        {
            return Result<BigNumber>(result.Value().Result());
        }
        return Result<BigNumber>::Err(result);
    }

    Result<BigNumber> Chain::GetTransactionCount(const Address address) const
    {
        Result<char *> result = MakeRequst("eth_getTransactionCount", {cJSON_CreateString(address.AsString()), cJSON_CreateString("latest")});

        if (result.HasValue())
        {
            BigNumber count(result.Value());
            delete []result.Value();
            return count;
        }

        return Result<BigNumber>::Err(result);
    }

    Result<TransactionReceipt*> Chain::GetTransactionReceipt(const char *transactionHash) const
    {
        Result<char *> result = MakeRequst("eth_getTransactionReceipt", {cJSON_CreateString(transactionHash)});

        if (result.HasValue())
        {
            if (result.Value() == NULL)
            {
                return Result<TransactionReceipt*>(nullptr);
            }
            else
            {
                cJSON *json = cJSON_Parse(result.Value());
                free(result.Value());
                Result<TransactionReceipt *> transactionReceipt = TransactionReceipt::Parse(json);
                cJSON_Delete(json);
                return transactionReceipt;
            }
        }

        return Result<TransactionReceipt *>::Err(result);
    }

    Result<BlockInformation*> Chain::GetBlockInformation(const char *blockHash) const
    {
        Result<char *> result = MakeRequst("eth_getBlockByHash", {cJSON_CreateString(blockHash), cJSON_CreateBool(false)});

        if (result.HasValue())
        {
            if (result.Value() == NULL)
            {
                return Result<BlockInformation *>(nullptr);
            }
            else
            {
                cJSON *json = cJSON_Parse(result.Value());
                free(result.Value());
                Result<BlockInformation *> blockInformation = BlockInformation::Parse(json);
                cJSON_Delete(json);
                return blockInformation;
            }
        }

        return Result<BlockInformation *>::Err(result);
    }

    Result<TransactionResponse> Chain::ViewCall(const Address callerAddress, const Address contractAddress, const ContractCall *contractCall) const
    {
        char *dataAsHexString = (contractCall->AsData() | byte_array::hex_string) | char_string::add_hex_prefix;
        cJSON *callCJson = cJSON_CreateObject();
        cJSON_AddStringToObject(callCJson, "from", callerAddress.AsString());
        cJSON_AddStringToObject(callCJson, "to", contractAddress.AsString());
        cJSON_AddStringToObject(callCJson, "data", dataAsHexString);
        delete []dataAsHexString;

        Result<char *> result = MakeRequst("eth_call", {callCJson, cJSON_CreateString("latest")});

        if (result.HasValue())
        {
            return Result<TransactionResponse>(result.Value());
        }
        return Result<TransactionResponse>::Err(result.ErrorCode(), result.ErrorMessage());
    }

    Result<TransactionResponse> Chain::Send(const Account *from, const Address to,
                                            const BigNumber amount, const uint32_t gasLimit,
                                            const BigNumber *gasPrice, const ContractCall *contractCall) const
    {
        AssertStarted();
        Result<BigNumber> nonceResult = GetTransactionCount(from->GetAddress());

        if (!nonceResult.HasValue())
        {
            return Result<TransactionResponse>::Err(-1, "Unable to retrieve nonce.");
        }

        uint32_t nonce = nonceResult.Value().ToUInt32();

        BigNumber gp(gasPrice);

        if (gasPrice == nullptr)
        {
            Result<BigNumber> gasPriceResult = GetGasPrice();
            if (!gasPriceResult.HasValue())
            {
                return Result<TransactionResponse>::Err(-41, "Unable to fetch gas price.");
            }
            gp = gasPriceResult.Value();
        }

        char *parameter = transactionFactory->GenerateSerializedData(from, EthereumTransactionProperties(nonce, gp, gasLimit, to, amount, (contractCall ? contractCall->AsData() : std::vector<uint8_t>()), id));
        
        Result<char *> result = MakeRequst("eth_sendRawTransaction", {cJSON_CreateString(parameter)});
        delete[] parameter;

        if (result.HasValue())
        {
            return Result<TransactionResponse>(result.Value());
        }
        return Result<TransactionResponse>::Err(result);
    }

    Result<BigNumber> Chain::EstimateGas(const Account *from, const Address to,
                                         const BigNumber amount, const uint32_t gasLimit,
                                         const BigNumber *gasPrice, const ContractCall *contractCall) const
    {
        AssertStarted();
        Result<BigNumber> nonceResult = GetTransactionCount(from->GetAddress());

        if (!nonceResult.HasValue())
        {
            return Result<BigNumber>::Err(-1, "Unable to retrieve nonce.");
        }

        uint32_t nonce = nonceResult.Value().ToUInt32();

        BigNumber gp(gasPrice);

        if (gasPrice == nullptr)
        {
            Result<BigNumber> gasPriceResult = GetGasPrice();
            if (!gasPriceResult.HasValue())
            {
                return Result<BigNumber>::Err(-41, "Unable to fetch gas price.");
            }
            gp = gasPriceResult.Value();
        }

        char *parameter = transactionFactory->GenerateSerializedData(from, EthereumTransactionProperties(nonce, gp, gasLimit, to, amount, (contractCall ? contractCall->AsData() : std::vector<uint8_t>()), id));
        Result<char *> result = MakeRequst("eth_estimateGas", {cJSON_CreateString(parameter)});
        delete[] parameter;

        if (result.HasValue())
        {
            Result<BigNumber> gasPrice(result.Value());
            delete[] result.Value();
            return gasPrice;
        }
        return Result<BigNumber>::Err(result);
    }

    Result<BigNumber> Chain::GetGasPrice() const
    {
        Result<char *> result = MakeRequst("eth_gasPrice", {});

        if (result.HasValue())
        {
            BigNumber count(result.Value());
            delete[] result.Value();
            return count;
        }

        return Result<BigNumber>::Err(result);
    }

    Result<BigNumber> Chain::GetBalance(const Address address) const
    {
        Result<char *> result = MakeRequst("eth_getBalance", {cJSON_CreateString(address.AsString()), cJSON_CreateString("latest")});

        if (result.HasValue())
        {
            BigNumber count(result.Value());
            delete[] result.Value();
            return count;
        }

        return Result<BigNumber>::Err(result);
    }

    Result<char *> Chain::MakeRequst(const char* method, const std::vector<cJSON *> parameters, const bool assertStarted) const {

        if (assertStarted) {
            AssertStarted();
        }
        
        cJSON *params = cJSON_CreateArray();
        for(cJSON *parameter : parameters) {
            cJSON_AddItemToArray(params, parameter);
        }

        char *request_body = BaseJsonBody(method, params);
        Result<char *> result = DoRequestYo(network, url, request_body);
        cJSON_free(request_body);
        return result;
    }

    void Chain::AssertStarted() const
    {
        if (!started)
        {
            THROW("Need to call Start() before operations!");
        }
    }
}
