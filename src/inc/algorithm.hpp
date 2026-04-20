// -------------------------------------------------------------
//
//!\file algorithm.hpp
//!\brief Algorithm metadata and execution types for Operon
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_ALGORITHM_HPP
#define OPERON_ALGORITHM_HPP

// system
#include <functional>
#include <string>
#include <vector>

// internal
#include "context.hpp"
#include "result.hpp"
#include "value.hpp"

namespace OPERON {

    //!\brief Function signature for an Operon algorithm
    using AlgorithmFunction = std::function<Result(Context&, const std::vector<Value>&)>;

    struct AlgorithmParameter
    {
        //!\brief Parameter name
        std::string name;

        //!\brief Parameter description
        std::string brief;

        //!\brief Parameter type
        ValueType type = ValueType::Null;

        //!\brief Whether the parameter is required
        bool required = true;
    };

    struct AlgorithmInfo
    {
        //!\brief Full algorithm name
        std::string name;

        //!\brief Algorithm category
        std::string category;

        //!\brief Short description
        std::string brief;

        //!\brief Input parameters
        std::vector<AlgorithmParameter> parameters;

        //!\brief Return type
        ValueType return_type = ValueType::Null;

        //!\brief Required algorithms
        std::vector<std::string> requirements;
    };

    struct Algorithm
    {
        //!\brief Algorithm metadata
        AlgorithmInfo info;

        //!\brief Algorithm implementation
        AlgorithmFunction function;
    };

} // namespace OPERON

#endif // OPERON_ALGORITHM_HPP