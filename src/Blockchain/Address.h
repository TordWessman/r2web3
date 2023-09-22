#ifndef __ADDRESS__H__
#define __ADDRESS__H__

namespace blockchain
{       
    #define ETH_ADDRESS_LENGTH (64 + 2)

    /// @brief Represents an address on the blockchain.
    class Address
    {
    public:
        Address(const char *addressString)
        {
            if (addressString[0] == '0' && (addressString[1] == 'x' || addressString[1] == 'X'))
            {
                for (int i = 0; i < ETH_ADDRESS_LENGTH; i++) { address[i] = addressString[i]; }
            }
            else
            {
                for (int i = 2; i < ETH_ADDRESS_LENGTH; i++) { address[i] = addressString[i - 2]; }
                address[0] = '0'; address[1] = 'x';
            }
            address[ETH_ADDRESS_LENGTH] = '\0';
        }

        const char* AsString() const { return address; }

    private:
        char address[ETH_ADDRESS_LENGTH + 1];
    };
}
#endif