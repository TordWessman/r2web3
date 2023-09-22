#ifndef __TRANSACTION_RESPONSE_H__
#define __TRANSACTION_RESPONSE_H__

#include <cstring>

namespace blockchain
{
    /// @brief Arbitrary response container for requests. Will contain the `"result"` value for an ethereum request.
    class TransactionResponse
    {

    public:
        /// @brief Empty response.
        TransactionResponse() : result(nullptr) {}

        /// @brief Default constructor. `result` must be a null-terminated string.
        TransactionResponse(char *result) : result(result) {}

        TransactionResponse(const TransactionResponse &other)
        {
            if (other.result != nullptr)
            {
                result = new char[strlen(other.result) + 1];
                result[strlen(other.result)] = '\0';
                memcpy(result, other.result, strlen(other.result));
            }
            else
            {
                result = nullptr;
            }
        }

        ~TransactionResponse()
        {
            if (result != nullptr)
            {
                delete[] result;
            }
            result = nullptr;
        }

        TransactionResponse *clone() const { return new TransactionResponse(*this); }

        TransactionResponse &operator=(const TransactionResponse &other)
        {
            if (this != &other)
            {
                if (other.result != nullptr)
                {
                    result = new char[strlen(other.result) + 1];
                    result[strlen(other.result)] = '\0';
                    memcpy(result, other.result, strlen(other.result));
                }
                else
                {
                    result = nullptr;
                }
            }
            return *this;
        }

        /// @brief Returns the (internally managed) `"result"` data.
        /// @return
        char *Result() { return result; }

    private:
        char *result;
    };
}
#endif