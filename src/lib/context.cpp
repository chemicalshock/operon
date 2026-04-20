// -------------------------------------------------------------
//
//!\file context.cpp
//!\brief Execution context implementation for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "context.hpp"

//
//!\brief Default constructor
//
OPERON::Context::Context()
    : m_runtime(nullptr)
{
}

//
//!\brief Construct a context bound to a runtime
//
OPERON::Context::Context(Runtime& runtime)
    : m_runtime(&runtime)
{
}

//
//!\brief Destructor
//
OPERON::Context::~Context() = default;

//
//!\brief Execute an algorithm through the bound runtime
//
OPERON::Result OPERON::Context::execute(const std::string& algorithm_name, const std::vector<Value>& args)
{
    if (m_runtime == nullptr)
    {
        return Result::error(
            "E101_CONTEXT_NOT_BOUND",
            "Context is not bound to a runtime"
        );
    }

    return m_runtime->execute(algorithm_name, args);
}

//
//!\brief Execute an algorithm through the bound runtime
//
OPERON::Result OPERON::Context::execute(const std::string& algorithm_name, std::initializer_list<Value> args)
{
    return execute(algorithm_name, std::vector<Value>(args));
}

//
//!\brief Check whether an algorithm exists in the bound runtime
//
bool OPERON::Context::has_algorithm(const std::string& algorithm_name) const
{
    if (m_runtime == nullptr)
    {
        return false;
    }

    return m_runtime->has_algorithm(algorithm_name);
}

//
//!\brief Get metadata for an algorithm in the bound runtime
//
bool OPERON::Context::get_algorithm_info(const std::string& algorithm_name, AlgorithmInfo& info) const
{
    if (m_runtime == nullptr)
    {
        return false;
    }

    return m_runtime->get_algorithm_info(algorithm_name, info);
}

//
//!\brief Check whether the context is bound to a runtime
//
bool OPERON::Context::is_bound() const
{
    return m_runtime != nullptr;
}
