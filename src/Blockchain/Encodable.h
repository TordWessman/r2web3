#ifndef __ENCODABLE_H__
#define __ENCODABLE_H__

#include <vector>
#include <stdint.h>

#include "../Shared/Common.h"
#include "../Shared/BigNumber.h"
#include "Address.h"

namespace blockchain
{
    /// @brief Supported encoding types.
    enum class EncodableItemType
    {
        Item,
        Binary,
        Address,
        String,
        ItemArray,
        UnsignedInt,
        Bool
    };

    /// Shortcut for the creation of an encodable item.
    #define ENC EncodableItem

    /// @brief Represents an object prepared for encoding.
    /// The encoded data could either be an array of `EncodableItem`'s or a concrete value-type.
    class EncodableItem
    {
    public:

        /// @brief Encode as a binary array.
        EncodableItem(const std::vector<uint8_t> value, const char* handle = "bytes") : type(EncodableItemType::Binary), bytes(value), handle(handle) {}

        /// @brief Encode as address
        EncodableItem(const Address *value) : type(EncodableItemType::Address), bytes(value->AsString() | byte_array::hex_string_to_bytes), handle("address") {}

        /// @brief Encode as a 32-bit unsigned integer. `handle` defaults to "uint256".
        EncodableItem(uint32_t value, const char *handle = "uint256") : type(EncodableItemType::UnsignedInt), bytes(value | byte_array::uint_to_bytes), handle(handle) {}

        /// @brief Encode as a 16-bit unsigned integer. `handle` defaults to "uint256".
        EncodableItem(uint16_t value, const char *handle = "uint256") : type(EncodableItemType::UnsignedInt), bytes((uint32_t)value | byte_array::uint_to_bytes), handle(handle) {}

        /// @brief Encode as a 8-bit unsigned integer. `handle` defaults to "uint256".
        EncodableItem(uint8_t value, const char *handle = "uint256") : type(EncodableItemType::UnsignedInt), bytes((uint32_t)value | byte_array::uint_to_bytes), handle(handle) {}

        /// @brief Encode any unsigned number.
        EncodableItem(const BigNumber *value, const char *handle = "uint256") : type(EncodableItemType::UnsignedInt), bytes(value->Bytes() | byte_array::truncate), handle(handle) {}

        /// @brief Encode a string.
        EncodableItem(const char *value) : type(EncodableItemType::String), bytes(value | byte_array::string_to_bytes), handle("string") {}

        /// @brief Encode as a bool.
        EncodableItem(const bool value) : type(EncodableItemType::Bool), bytes(std::vector<uint8_t> {(uint8_t)value}), handle("uint8") {}

        /// @brief Encode a list of sub-items.
        EncodableItem(const std::vector<EncodableItem> items, const char* handle = "") : type(EncodableItemType::ItemArray), children(items), handle(handle) {}
        
        /// @brief Return the original type of the encoded content. 
        EncodableItemType Type() const { return type; }

        /// @brief Return as encoded bytes. 
        std::vector<uint8_t> Bytes() const { return bytes; }

        /// @brief Returns any sub-items of this is an array. 
        std::vector<EncodableItem> Children() const { return children; }

        /// @brief Returns the encoded handle. This value is being used for ABI-encoding.
        const char* Handle() const { return handle; }

    private:
        const EncodableItemType type;
        const std::vector<uint8_t> bytes;
        const std::vector<EncodableItem> children;
        const char *handle;
    };

    /// @brief Interface for encoding `EncodableItem`'s
    class EncoderX
    {
    public:
        virtual ~EncoderX() {}
        virtual EncoderX *clone() const
    #ifndef ARDUINO
        { throw std::runtime_error("EncoderX::clone() must be overloaded"); }
    #else
            = 0;
    #endif

        /// @brief Serializes the item and it's components.
        /// @param item 
        /// @return A byte-array containing the serialized item.
        virtual std::vector<uint8_t> Encode(const EncodableItem *item) const
    #ifndef ARDUINO
        { throw std::runtime_error("EncoderX::Encode(EncodableItem item) must be overloaded"); }; 
    #else
            = 0;
    #endif
    };
}
#endif