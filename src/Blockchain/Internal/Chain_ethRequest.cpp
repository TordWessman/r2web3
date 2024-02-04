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

#include "../../Network/HttpResponse.h"
#include "../../Network/HttpRequest.h"
#include "../../Shared/R2Web3Log.h"
#include "Chain_ethRequest.h"

namespace blockchain
{
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
                Result<char *>::Err(-2, "Unable to parse JSON due to unknown error");
            }

            return Result<char *>::Err(-2, errorPtr);
        }
        else
        {
            cJSON *resultJson = cJSON_GetObjectItemCaseSensitive(response_json, "result");
            
            if (resultJson != NULL)
            {
                if (cJSON_IsString(resultJson))
                {
                    Result<char *> result(resultJson->valuestring | char_string::retain);
                    cJSON_Delete(response_json);
                    return result;
                }
                else
                {
                    char *json_str = cJSON_Print(resultJson);
                    if (json_str | char_string::is_null)
                    {
                        cJSON_Delete(response_json);
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
                char *message = cJSON_GetObjectItem(resultJson, "message")->valuestring | char_string::retain;
                Result<char *> result = Result<char *>::Err(code, message);
                cJSON_Delete(response_json);
                return result;
            }
        }

        Log::e("JSON ERROR", cJSON_Print(response_json));
        cJSON_Delete(response_json);
        return Result<char *>::Err(-3, "Invalid JSON");
    }
}