// -------------------------------------------------------------
//
//!\file value.hpp
//!\brief Generic value container for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_VALUE_HPP
#define OPERON_VALUE_HPP

// system
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace OPERON {

    enum class ValueType
    {
        Null,
        Bool,
        Int,
        Float,
        String,
        Buffer,
        Array
    };

    class Value
    {
    public:
        using Buffer = std::vector<uint8_t>;
        using Array  = std::vector<Value>;

        //!\brief Construct null
        Value();

        //!\brief Construct bool
        Value(bool v);

        //!\brief Construct int
        Value(int v);

        //!\brief Construct int
        Value(unsigned int v);

        //!\brief Construct int
        Value(long long v);

        //!\brief Construct int
        Value(unsigned long long v);

        //!\brief Construct float
        Value(float v);

        //!\brief Construct float
        Value(double v);

        //!\brief Construct string
        Value(const char* v);

        //!\brief Construct string
        Value(const std::string& v);

        //!\brief Construct buffer
        Value(const Buffer& v);

        //!\brief Construct array
        Value(const Array& v);

        //!\brief Get type
        ValueType type() const;

        //!\brief Check whether the value is null
        bool is_null() const;

        //!\brief Check whether the value is bool
        bool is_bool() const;

        //!\brief Check whether the value is int
        bool is_int() const;

        //!\brief Check whether the value is float
        bool is_float() const;

        //!\brief Check whether the value is string
        bool is_string() const;

        //!\brief Check whether the value is buffer
        bool is_buffer() const;

        //!\brief Check whether the value is array
        bool is_array() const;

        //!\brief Return the value as bool
        bool as_bool() const;

        //!\brief Return the value as int
        int64_t as_int() const;

        //!\brief Return the value as float
        double as_float() const;

        //!\brief Return the value as string
        const std::string& as_string() const;

        //!\brief Return the value as buffer
        const Buffer& as_buffer() const;

        //!\brief Return the value as array
        const Array& as_array() const;

    private:
        ValueType m_type;

        std::variant<
            std::monostate,
            bool,
            int64_t,
            double,
            std::string,
            Buffer,
            Array
        > m_data;
    };

} // namespace OPERON

#endif // OPERON_VALUE_HPP