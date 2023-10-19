#include "ABIDecoder.h"
#include "Common.h"

namespace blockchain
{
    Address ABIDecoder::DecodeAddress(const char *result)
    {
        char addr[strlen(result)];
        memcpy(&addr, result, strlen(result));

        return Address(addr | byte_array::hex_string_to_bytes);
    }
}