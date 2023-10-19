#ifndef __ABIDecoder_H__
#define __ABIDecoder_H__

#include <vector>

#include "TransactionResponse.h"
#include "Encodable.h"
#include "Address.h"

namespace blockchain
{

    /// @brief Default interface for decoding ABI information
    class ABIDecoder
    {

    public:
        /// @brief Decode raw `result` data into an `Address`.
        /// @param result 
        /// @return 
        Address DecodeAddress(const char *result);
    };
}


#endif