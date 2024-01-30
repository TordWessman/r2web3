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

namespace blockchain
{
    bool Chain::Start()
    {
        if (id == 0)
        {
            Result<uint32_t> chainIdResult = GetChainId();
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

    char *BaseJsonBody(const char *method, cJSON *params)
    {
        Log::m("Preparing request:", method);
        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "method", method);
        cJSON_AddNumberToObject(json, "id", 1);
        cJSON_AddStringToObject(json, "jsonrpc", "2.0");
        if (params != NULL)
        {
            cJSON_AddItemToObject(json, "params", params);
        }
        char *json_str = cJSON_Print(json);

        cJSON_Delete(json);

        return json_str;
    }

    Result<char *> DoRequestYo(const NetworkFacade *network, const char *url, const char *request_body)
    {
        HttpRequest request = HttpRequest(network);
        request.SetMethod("POST");
        request.SetBody(request_body);

        HttpResponse response = request.SendRequest(url);

        if (!response.Success())
        {
            return Result<char *>::Err(response.status, "HTTP Error");
        }

        cJSON *response_json = NULL;
        if (response.GetBody() != nullptr)
        {
            response_json = cJSON_Parse(response.GetBody());
        }

        if (response_json == NULL)
        {
            const char *errorPtr = cJSON_GetErrorPtr();
            if (errorPtr != NULL)
            {
                Log::e("Unable to parse:", errorPtr);
            }
            else
            {
                Log::e("Unable to parse. Null response?");
            }

            return Result<char *>::Err(-2, "Error parsing response as JSON");
        }
        else
        {
            cJSON *resultJson = cJSON_GetObjectItemCaseSensitive(response_json, "result");

            if (resultJson != NULL) 
            {
                if (cJSON_IsString(resultJson))
                {
                    char *resultString = (char *)malloc(strlen(resultJson->valuestring) + 1);
                    memset(resultString, 0, strlen(resultJson->valuestring) + 1);
                    memcpy(resultString, resultJson->valuestring, strlen(resultJson->valuestring));
                    Result<char *> result(resultString);
                    Log::m("Result:", resultJson->valuestring);
                    cJSON_Delete(response_json);
                    return result;
                }
                else
                {
                    char *json_str = cJSON_Print(resultJson);
                    if (json_str | char_string::is_null)
                    {
                        free(json_str);
                        return Result<char *>(nullptr);
                    }
                    Result<char *> result(json_str);
                    cJSON_Delete(response_json);
                    return result;
                }
            }

            resultJson = cJSON_GetObjectItemCaseSensitive(response_json, "error");

            if (resultJson != NULL)
            {
                int code = cJSON_GetObjectItem(resultJson, "code")->valueint;
                char *message = cJSON_GetObjectItem(resultJson, "message")->valuestring;
                Result<char *> result = Result<char *>::Err(code, message);
                cJSON_Delete(response_json);
                return result;
            }
            else
            {
                Log::e("No 'error'. No 'result'");
            }
        }

        cJSON_Delete(response_json);
        Log::e("JSON ERROR", cJSON_Print(response_json));
        return Result<char *>::Err(-3, "Invalid JSON");
    }

    Result<uint32_t> Chain::GetChainId() const
    {
        cJSON *params = cJSON_CreateArray();

        char *request_body = BaseJsonBody("eth_chainId", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        free(request_body);

        if (result.HasValue())
        {
            uint32_t value = strtol(result.Value(), nullptr, 16);
            free(result.Value());
            return Result<uint32_t>(value);
        }

        return Result<uint32_t>::Err(result.ErrorCode(), result.ErrorMessage());
    }

    Result<BigNumber> Chain::GetBalance(const Address address, const Address contractAddress) const
    {
        ContractCall getBalanceCall("balanceOf", {ENC(address)});
        Result<TransactionResponse> response = ViewCall(address, contractAddress, &getBalanceCall);
        if (response.HasValue())
        {
            return Result<BigNumber>(response.Value().Result());
        }
        return Result<BigNumber>::Err(response.ErrorCode(), response.ErrorMessage());
    }

    Result<BigNumber> Chain::GetTransactionCount(const Address address) const
    {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();

        cJSON_AddItemToArray(params, cJSON_CreateString(address.AsString()));
        cJSON_AddItemToArray(params, cJSON_CreateString("latest"));

        char *request_body = BaseJsonBody("eth_getTransactionCount", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            BigNumber count(result.Value());
            free(result.Value());
            return count;
        }

        return Result<BigNumber>::Err(result.ErrorCode());
    }

    Result<TransactionReceipt*> Chain::GetTransactionReceipt(const char *transactionHash) const
    {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();
        cJSON_AddItemToArray(params, cJSON_CreateString(transactionHash));
        char *request_body = BaseJsonBody("eth_getTransactionReceipt", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

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
                return Result<TransactionReceipt*>(new TransactionReceipt(json));
            }
        }
        
        return Result<TransactionReceipt*>::Err(result.ErrorCode(), result.ErrorMessage());
    }

    Result<TransactionResponse> Chain::ViewCall(const Address callerAddress, const Address contractAddress, const ContractCall *contractCall) const
    {
        AssertStarted();
        cJSON *callCJson = cJSON_CreateObject();
        cJSON *params = cJSON_CreateArray();

        cJSON_AddStringToObject(callCJson, "from", callerAddress.AsString());
        cJSON_AddStringToObject(callCJson, "to", contractAddress.AsString());
        char *dataAsHexString = (contractCall->AsData() | byte_array::hex_string) | char_string::add_hex_prefix;

        cJSON_AddStringToObject(callCJson, "data", dataAsHexString);
        free(dataAsHexString);
        cJSON_AddItemToArray(params, callCJson);
        cJSON_AddItemToArray(params, cJSON_CreateString("latest"));

        char *request_body = BaseJsonBody("eth_call", params);

        Result<char *> result = DoRequestYo(network, url, request_body);
        cJSON_free(request_body);

        if (result.HasValue())
        {
            return Result<TransactionResponse>(result.Value());
        }
        return Result<TransactionResponse>::Err(result.ErrorCode());
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

        EthereumTransactionProperties p(nonce, gp, gasLimit, to, amount, (contractCall ? contractCall->AsData() : std::vector<uint8_t>()), id);

        std::vector<uint8_t> pk = from->GetPrivateKey();

        EthereumTransaction tx = transactionFactory->GenerateTransaction(p, &pk);

        std::vector<uint8_t> serializedTransaction = tx.Serialize();

        cJSON *params = cJSON_CreateArray();
        char *parameter = (serializedTransaction | byte_array::hex_string) | char_string::add_hex_prefix;
        cJSON_AddItemToArray(params, cJSON_CreateString(parameter));
        Log::m("Transaciton data: ", parameter);
        delete[] parameter;

        char *request_body = BaseJsonBody("eth_sendRawTransaction", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            return Result<TransactionResponse>(result.Value());
        }
        return Result<TransactionResponse>::Err(result.ErrorCode());
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

        EthereumTransactionProperties p(nonce, gp, gasLimit, to, amount, (contractCall ? contractCall->AsData() : std::vector<uint8_t>()), id);

        std::vector<uint8_t> pk = from->GetPrivateKey();

        EthereumTransaction tx = transactionFactory->GenerateTransaction(p, &pk);

        std::vector<uint8_t> serializedTransaction = tx.Serialize();

        cJSON *params = cJSON_CreateArray();
        char *parameter = (serializedTransaction | byte_array::hex_string) | char_string::add_hex_prefix;
        cJSON_AddItemToArray(params, cJSON_CreateString(parameter));
        delete[] parameter;

        char *request_body = BaseJsonBody("eth_estimateGas", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            Result<BigNumber> gasPrice(result.Value());
            free(result.Value());
            return gasPrice;
        }
        return Result<BigNumber>::Err(result.ErrorCode());
    }

    Result<BigNumber> Chain::GetGasPrice() const
    {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();

        char *request_body = BaseJsonBody("eth_gasPrice", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            BigNumber count(result.Value());
            free(result.Value());
            return count;
        }

        return Result<BigNumber>::Err(result.ErrorCode());
    }

    Result<BigNumber> Chain::GetBalance(const Address address) const
    {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();

        cJSON_AddItemToArray(params, cJSON_CreateString(address.AsString()));
        cJSON_AddItemToArray(params, cJSON_CreateString("latest"));

        char *request_body = BaseJsonBody("eth_getBalance", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            BigNumber count(result.Value());
            free(result.Value());
            return count;
        }

        return Result<BigNumber>::Err(result.ErrorCode());
    }

    void Chain::AssertStarted() const
    {
        if (!started)
        {
            THROW("Need to call Start() before operations!");
        }
    }
}
