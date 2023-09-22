#include "Chain.h"
#include "../Network/HttpRequest.h"

namespace blockchain
{
    bool Chain::Start()
    {
        if (id == 0) 
        {  
            Result<uint32_t> chainIdResult = GetChainId();
            if (chainIdResult.HasValue()) { id = chainIdResult.Value(); } 
            else 
            {
                Log::m("Unable to fetch chainId");
                return false;
            }
        }
        started = true;
        return true;
    }


    char* BaseJsonBody(const char *method, cJSON* params) 
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

        cJSON *response_json = cJSON_Parse(response.body);

        if (response_json == NULL)
        {
            cJSON_Delete(response_json);
            const char *errorPtr = cJSON_GetErrorPtr();
            if (errorPtr != NULL)
            {
                Log::m("Unable to parse:", errorPtr);
                return Result<char *>::Err(-2, "Error parsing response as JSON");
            }
        }
        else
        {
            cJSON *resultJson = cJSON_GetObjectItemCaseSensitive(response_json, "result");

            if (cJSON_IsString(resultJson) && (resultJson->valuestring != NULL))
            {
                char* resultString = (char*)malloc(strlen(resultJson->valuestring) + 1);
                memset(resultString, 0, strlen(resultJson->valuestring) + 1);
                memcpy(resultString, resultJson->valuestring, strlen(resultJson->valuestring));
                Result<char *> result(resultString);
                Log::m("Result:", resultJson->valuestring);
                cJSON_Delete(response_json);
                return result;
            }

            resultJson = cJSON_GetObjectItemCaseSensitive(response_json, "error");

            if (resultJson != NULL)
            {
                char *json_str = cJSON_Print(resultJson);
                Result<char *> result = Result<char *>::Err(-1, "Unable to parse `error` from JSON.");
                Log::m("JSON ERROR", json_str);
                cJSON_Delete(response_json);
                cJSON_free(json_str);
                return result;
            }
            else { Log::m("No 'error'. No 'result'"); }
        }

        cJSON_Delete(response_json);
        Log::m("JSON ERROR", cJSON_Print(response_json));
        return Result<char *>::Err(-3, "Invalid JSON");
    }

    Result<uint32_t> Chain::GetChainId() const 
    {
        cJSON *params = cJSON_CreateArray();

        char* request_body = BaseJsonBody("eth_chainId", params);

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

    Result<BigUnsigned> Chain::GetTransactionCount(const Account *account) const
    {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();

        cJSON_AddItemToArray(params, cJSON_CreateString(account->GetAddress().AsString()));
        cJSON_AddItemToArray(params, cJSON_CreateString("latest"));

        char* request_body = BaseJsonBody("eth_getTransactionCount", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            BigUnsigned count(result.Value());
            free(result.Value());
            return count;
        }

        return Result<BigUnsigned>::Err(result.ErrorCode());
    }

    Result<TransactionResponse> Chain::ViewCall(const ContractCall *contractCall, const Address &contractAddress) const
    {
        AssertStarted();
        cJSON *callCJson = cJSON_CreateObject();
        cJSON *params = cJSON_CreateArray();

        cJSON_AddNullToObject(callCJson, "from");
        cJSON_AddStringToObject(callCJson, "to", contractAddress.AsString());
        char *dataAsHexString = (contractCall->AsData() | byte_array::hex_string) | char_string::add_hex_prefix;
        cJSON_AddStringToObject(callCJson, "data", dataAsHexString);
        free(dataAsHexString);
        cJSON_AddItemToArray(params, callCJson);
        cJSON_AddItemToArray(params, cJSON_CreateString("latest"));

        char *request_body =  BaseJsonBody("eth_call", params);

        Result<char *> result = DoRequestYo(network, url, request_body);
        cJSON_free(request_body);

        if (result.HasValue())
        {
            return Result<TransactionResponse>(result.Value());
        }
        return Result<TransactionResponse>::Err(result.ErrorCode());
        
    }

    Result<TransactionResponse> Chain::Send(const Account *from, const Address &to,
                                            const BigUnsigned &amount, const BigUnsigned &gasPrice,
                                            const uint32_t gasLimit, const ContractCall *contractCall) const
    {
        AssertStarted();
        Result<BigUnsigned> nonceResult = GetTransactionCount(from);
        if (!nonceResult.HasValue())  {
            return Result<TransactionResponse>::Err(-1, "Unable to retrieve nonce.");
        }

        uint32_t nonce = nonceResult.Value().ToUInt32();
        
        EthereumTransactionProperties p(nonce, gasPrice, gasLimit, to, amount, (contractCall ? contractCall->AsData() : std::vector<uint8_t>()), id);

        std::vector<uint8_t> pk = from->GetPrivateKey();
        
        EthereumTransaction tx = transactionFactory->GenerateTransaction(p, &pk);
        
        std::vector<uint8_t> vvvv = tx.Serialize();
        
        cJSON *params = cJSON_CreateArray();
        char *parameter = (vvvv | byte_array::hex_string) | char_string::add_hex_prefix;
        cJSON_AddItemToArray(params, cJSON_CreateString(parameter)); //parameter.c_str()
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

    Result<BigUnsigned> Chain::GetGasPrice() const  {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();

        char* request_body = BaseJsonBody("eth_gasPrice", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            BigUnsigned count(result.Value());
            free(result.Value());
            return count;
        }

        return Result<BigUnsigned>::Err(result.ErrorCode());
    }

    Result<BigUnsigned> Chain::GetBalance(const Account *account) const {
        AssertStarted();
        cJSON *params = cJSON_CreateArray();

        cJSON_AddItemToArray(params, cJSON_CreateString(account->GetAddress().AsString()));
        cJSON_AddItemToArray(params, cJSON_CreateString("latest"));

        char* request_body = BaseJsonBody("eth_getBalance", params);

        Result<char *> result = DoRequestYo(network, url, request_body);

        cJSON_free(request_body);

        if (result.HasValue())
        {
            BigUnsigned count(result.Value());
            free(result.Value());
            return count;
        }

        return Result<BigUnsigned>::Err(result.ErrorCode());
    }

    void Chain::AssertStarted() const {
        if(!started) { THROW("Need to call Start() before operations!"); }
    }
}