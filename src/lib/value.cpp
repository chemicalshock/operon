// -------------------------------------------------------------
//
//!\file value.cpp
//!\brief Generic value container implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "value.hpp"

// system
#include <stdexcept>
#include <string>

//
//!\brief Construct null
//
OPERON::Value::Value()
    : m_type(ValueType::Null),
      m_data(std::monostate())
{
}

//
//!\brief Construct bool
//
OPERON::Value::Value(bool v)
    : m_type(ValueType::Bool),
      m_data(v)
{
}

//
//!\brief Construct int
//
OPERON::Value::Value(int v)
    : m_type(ValueType::Int),
      m_data(static_cast<int64_t>(v))
{
}

//
//!\brief Construct int
//
OPERON::Value::Value(unsigned int v)
    : m_type(ValueType::Int),
      m_data(static_cast<int64_t>(v))
{
}

//
//!\brief Construct int
//
OPERON::Value::Value(long long v)
    : m_type(ValueType::Int),
      m_data(static_cast<int64_t>(v))
{
}

//
//!\brief Construct int
//
OPERON::Value::Value(unsigned long long v)
    : m_type(ValueType::Int),
      m_data(static_cast<int64_t>(v))
{
}

//
//!\brief Construct float
//
OPERON::Value::Value(float v)
    : m_type(ValueType::Float),
      m_data(static_cast<double>(v))
{
}

//
//!\brief Construct float
//
OPERON::Value::Value(double v)
    : m_type(ValueType::Float),
      m_data(v)
{
}

//
//!\brief Construct string
//
OPERON::Value::Value(const char* v)
    : m_type(ValueType::String),
      m_data(std::string(v != nullptr ? v : ""))
{
}

//
//!\brief Construct string
//
OPERON::Value::Value(const std::string& v)
    : m_type(ValueType::String),
      m_data(v)
{
}

//
//!\brief Construct buffer
//
OPERON::Value::Value(const Buffer& v)
    : m_type(ValueType::Buffer),
      m_data(v)
{
}

//
//!\brief Construct array
//
OPERON::Value::Value(const Array& v)
    : m_type(ValueType::Array),
      m_data(v)
{
}

//
//!\brief Get type
//
OPERON::ValueType OPERON::Value::type() const
{
    return m_type;
}

//
//!\brief Check whether the value is null
//
bool OPERON::Value::is_null() const
{
    return m_type == ValueType::Null;
}

//
//!\brief Check whether the value is bool
//
bool OPERON::Value::is_bool() const
{
    return m_type == ValueType::Bool;
}

//
//!\brief Check whether the value is int
//
bool OPERON::Value::is_int() const
{
    return m_type == ValueType::Int;
}

//
//!\brief Check whether the value is float
//
bool OPERON::Value::is_float() const
{
    return m_type == ValueType::Float;
}

//
//!\brief Check whether the value is string
//
bool OPERON::Value::is_string() const
{
    return m_type == ValueType::String;
}

//
//!\brief Check whether the value is buffer
//
bool OPERON::Value::is_buffer() const
{
    return m_type == ValueType::Buffer;
}

//
//!\brief Check whether the value is array
//
bool OPERON::Value::is_array() const
{
    return m_type == ValueType::Array;
}

//
//!\brief Return the value as bool
//
bool OPERON::Value::as_bool() const
{
    if (!is_bool())
    {
        throw std::runtime_error("OPERON::Value is not a bool");
    }

    return std::get<bool>(m_data);
}

//
//!\brief Return the value as int
//
int64_t OPERON::Value::as_int() const
{
    if (!is_int())
    {
        throw std::runtime_error("OPERON::Value is not an int");
    }

    return std::get<int64_t>(m_data);
}

//
//!\brief Return the value as float
//
double OPERON::Value::as_float() const
{
    if (!is_float())
    {
        throw std::runtime_error("OPERON::Value is not a float");
    }

    return std::get<double>(m_data);
}

//
//!\brief Return the value as string
//
const std::string& OPERON::Value::as_string() const
{
    if (!is_string())
    {
        throw std::runtime_error("OPERON::Value is not a string");
    }

    return std::get<std::string>(m_data);
}

//
//!\brief Return the value as buffer
//
const OPERON::Value::Buffer& OPERON::Value::as_buffer() const
{
    if (!is_buffer())
    {
        throw std::runtime_error("OPERON::Value is not a buffer");
    }

    return std::get<Buffer>(m_data);
}

//
//!\brief Return the value as array
//
const OPERON::Value::Array& OPERON::Value::as_array() const
{
    if (!is_array())
    {
        throw std::runtime_error("OPERON::Value is not an array");
    }

    return std::get<Array>(m_data);
}