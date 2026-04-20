// -------------------------------------------------------------
//
//!\file operon.hpp
//!\brief Main public header for the Operon algorithm runtime
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_HPP
#define OPERON_HPP

// system
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

// internal
#include "algorithm.hpp"
#include "context.hpp"
#include "result.hpp"
#include "value.hpp"

namespace OPERON {

    class Registry;

    //!\brief Main public interface for the Operon algorithm runtime
    class Operon : private Context::Runtime
    {
    public:
        //!\brief Construct a new Operon runtime
        Operon();

        //!\brief Destroy the Operon runtime
        ~Operon();

        //!\brief Disable copy construction
        Operon(const Operon&) = delete;

        //!\brief Disable copy assignment
        Operon& operator=(const Operon&) = delete;

        //!\brief Enable move construction
        Operon(Operon&& other) noexcept;

        //!\brief Enable move assignment
        Operon& operator=(Operon&& other) noexcept;

        //!\brief Register built-in algorithms with the runtime
        bool register_builtins();

        //!\brief Register a single algorithm with the runtime
        bool register_algorithm(const Algorithm& algorithm);

        //!\brief Register a single algorithm registry with the runtime
        bool register_registry(std::shared_ptr<Registry> registry);

        //!\brief Execute an algorithm by name using positional arguments
        Result execute(const std::string& algorithm_name, std::initializer_list<Value> args);

        //!\brief Execute an algorithm by name using positional arguments
        Result execute(const std::string& algorithm_name, const std::vector<Value>& args = {}) override;

        //!\brief Execute an algorithm by name using a single input value
        Result execute(const std::string& algorithm_name, const Value& arg);

        //!\brief Check whether an algorithm exists
        bool has_algorithm(const std::string& algorithm_name) const override;

        //!\brief Return a list of all registered algorithm names
        std::vector<std::string> list_algorithms() const;

        //!\brief Return information for a specific algorithm
        bool get_algorithm_info(const std::string& algorithm_name, AlgorithmInfo& info) const override;

        //!\brief Return the runtime context
        Context& context();

        //!\brief Return the runtime context
        const Context& context() const;

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;
    };

} // namespace OPERON

#endif
