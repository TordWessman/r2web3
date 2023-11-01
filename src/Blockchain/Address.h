#ifndef __ADDRESS__H__
#define __ADDRESS__H__

#include <vector>

namespace blockchain
{       
    #define ETH_ADDRESS_LENGTH (64 + 2)

    /// @brief Represents a blockchain address.
    class Address
    {
    public:
        /// @brief Create an `Address` using the last `ETH_ADDRESS_LENGTH` bytes.
        /// @param bytes
        Address(const std::vector<uint8_t> *bytes)
        {
            Initialize(bytes);
        }

        /// @brief Create an `Address` using the last `ETH_ADDRESS_LENGTH` bytes.
        /// @param bytes
        Address(const std::vector<uint8_t> bytes)
        {
            Initialize(&bytes);
        }


        /// @brief Create an `Address` using its hex-representation (with or without 0x prefix).
        /// @param addressString 
        Address(const char *addressString)
        {
            if (addressString[0] == '0' && (addressString[1] == 'x' || addressString[1] == 'X'))
            {
                for (size_t i = 0; i < ETH_ADDRESS_LENGTH; i++) { address[i] = addressString[i]; }
            }
            else
            {
                for (size_t i = 2; i < ETH_ADDRESS_LENGTH; i++) { address[i] = addressString[i - 2]; }
                address[0] = '0'; address[1] = 'x';
            }
            address[ETH_ADDRESS_LENGTH] = '\0';
        }

        const char* AsString() const { return address; }

    private:
        
        char address[ETH_ADDRESS_LENGTH + 1];

        void Initialize(const std::vector<uint8_t> *bytes)
        {
            const size_t len = (ETH_ADDRESS_LENGTH - 2) / 2;
            assert(bytes->size() >=  len);
            std::copy(bytes->begin() + bytes->size() - len, bytes->end() - len, address);
            address[ETH_ADDRESS_LENGTH] = '\0';
            address[0] = '0'; address[1] = 'x';
        }
    };
}
#endif