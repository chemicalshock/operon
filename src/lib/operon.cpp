// -------------------------------------------------------------
//
//!\file operon.cpp
//!\brief Main public implementation for the Operon algorithm runtime
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "operon.hpp"

// system
#include <unordered_map>
#include <utility>

class OPERON::Operon::Impl
{
public:
    OPERON::Context m_context;
    std::unordered_map<std::string, OPERON::Algorithm> m_algorithms;
};

//
//!\brief Construct a new Operon runtime
//
OPERON::Operon::Operon()
    : m_impl(std::make_unique<Impl>())
{
}

//
//!\brief Destroy the Operon runtime
//
OPERON::Operon::~Operon() = default;

//
//!\brief Enable move construction
//
OPERON::Operon::Operon(Operon&& other) noexcept
    : m_impl(std::move(other.m_impl))
{
}

//
//!\brief Enable move assignment
//
OPERON::Operon& OPERON::Operon::operator=(Operon&& other) noexcept
{
    if (this != &other)
    {
        m_impl = std::move(other.m_impl);
    }

    return *this;
}

//
//!\brief Register built-in algorithms with the runtime
//
bool OPERON::Operon::register_builtins()
{
    Algorithm echo;

    echo.info.name = "test.echo";
    echo.info.category = "test";
    echo.info.brief = "Return the first input value unchanged";
    echo.info.parameters = {
        { "input", "Value to echo back", ValueType::String, true }
    };
    echo.info.return_type = ValueType::String;
    echo.function = [](Context&, const std::vector<Value>& args) -> Result
    {
        if (args.empty())
        {
            return Result::error("E001_MISSING_ARGUMENT", "test.echo requires one argument");
        }

        return Result::success(args[0]);
    };

    m_impl->m_algorithms[echo.info.name] = echo;

    Algorithm count_args;

    count_args.info.name = "test.count_args";
    count_args.info.category = "test";
    count_args.info.brief = "Return the number of supplied arguments";
    count_args.info.return_type = ValueType::Int;
    count_args.function = [](Context&, const std::vector<Value>& args) -> Result
    {
        return Result::success(OPERON::Value(static_cast<long long>(args.size())));
    };

    m_impl->m_algorithms[count_args.info.name] = count_args;

    return true;
}

//
//!\brief Register a single algorithm registry with the runtime
//
bool OPERON::Operon::register_registry(std::shared_ptr<Registry> registry)
{
    (void)registry;
    return false;
}

//
//!\brief Execute an algorithm by name using positional arguments
//
OPERON::Result OPERON::Operon::execute(const std::string& algorithm_name, std::initializer_list<Value> args)
{
    return execute(algorithm_name, std::vector<Value>(args));
}

//
//!\brief Execute an algorithm by name using positional arguments
//
OPERON::Result OPERON::Operon::execute(const std::string& algorithm_name, const std::vector<Value>& args)
{
    auto it = m_impl->m_algorithms.find(algorithm_name);

    if (it == m_impl->m_algorithms.end())
    {
        return Result::error("E002_ALGORITHM_NOT_FOUND", "Algorithm not found: " + algorithm_name);
    }

    if (!it->second.function)
    {
        return Result::error("E003_ALGORITHM_INVALID", "Algorithm has no implementation: " + algorithm_name);
    }

    return it->second.function(m_impl->m_context, args);
}

//
//!\brief Execute an algorithm by name using a single input value
//
OPERON::Result OPERON::Operon::execute(const std::string& algorithm_name, const Value& arg)
{
    return execute(algorithm_name, std::vector<Value>{ arg });
}

//
//!\brief Check whether an algorithm exists
//
bool OPERON::Operon::has_algorithm(const std::string& algorithm_name) const
{
    return m_impl->m_algorithms.find(algorithm_name) != m_impl->m_algorithms.end();
}

//
//!\brief Return a list of all registered algorithm names
//
std::vector<std::string> OPERON::Operon::list_algorithms() const
{
    std::vector<std::string> names;
    names.reserve(m_impl->m_algorithms.size());

    for (const auto& entry : m_impl->m_algorithms)
    {
        names.push_back(entry.first);
    }

    return names;
}

//
//!\brief Return information for a specific algorithm
//
bool OPERON::Operon::get_algorithm_info(const std::string& algorithm_name, AlgorithmInfo& info) const
{
    auto it = m_impl->m_algorithms.find(algorithm_name);

    if (it == m_impl->m_algorithms.end())
    {
        return false;
    }

    info = it->second.info;
    return true;
}

//
//!\brief Return the runtime context
//
OPERON::Context& OPERON::Operon::context()
{
    return m_impl->m_context;
}

//
//!\brief Return the runtime context
//
const OPERON::Context& OPERON::Operon::context() const
{
    return m_impl->m_context;
}