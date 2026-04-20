// -------------------------------------------------------------
//
//!\file context.hpp
//!\brief Execution context for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_CONTEXT_HPP
#define OPERON_CONTEXT_HPP

// system
#include <initializer_list>
#include <string>
#include <vector>

// internal
#include "result.hpp"
#include "value.hpp"

namespace OPERON {

    struct AlgorithmInfo;

    class Context
    {
    public:
        class Runtime
        {
        public:
            virtual ~Runtime() = default;

            //!\brief Execute an algorithm through the bound runtime
            virtual Result execute(const std::string& algorithm_name, const std::vector<Value>& args) = 0;

            //!\brief Check whether an algorithm exists in the bound runtime
            virtual bool has_algorithm(const std::string& algorithm_name) const = 0;

            //!\brief Get metadata for an algorithm in the bound runtime
            virtual bool get_algorithm_info(const std::string& algorithm_name, AlgorithmInfo& info) const = 0;
        };

        //!\brief Default constructor
        Context();

        //!\brief Construct a context bound to a runtime
        explicit Context(Runtime& runtime);

        //!\brief Destructor
        ~Context();

        //!\brief Execute an algorithm through the bound runtime
        Result execute(const std::string& algorithm_name, const std::vector<Value>& args = {});

        //!\brief Execute an algorithm through the bound runtime
        Result execute(const std::string& algorithm_name, std::initializer_list<Value> args);

        //!\brief Check whether an algorithm exists in the bound runtime
        bool has_algorithm(const std::string& algorithm_name) const;

        //!\brief Get metadata for an algorithm in the bound runtime
        bool get_algorithm_info(const std::string& algorithm_name, AlgorithmInfo& info) const;

        //!\brief Check whether the context is bound to a runtime
        bool is_bound() const;

    private:
        friend class Operon;

        //!\brief Bound runtime
        Runtime* m_runtime;
    };

} // namespace OPERON

#endif // OPERON_CONTEXT_HPP
