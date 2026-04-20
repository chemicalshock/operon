// -------------------------------------------------------------
//
//!\file result.hpp
//!\brief Result type for Operon execution
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

#ifndef OPERON_RESULT_HPP
#define OPERON_RESULT_HPP

// system
#include <string>

// internal
#include "value.hpp"

namespace OPERON {

    struct Result
    {
        //!\brief Success flag
        bool ok = false;

        //!\brief Result value
        Value value;

        //!\brief Error code
        std::string error_code;

        //!\brief Error message
        std::string error_message;

        //!\brief Create success result
        static Result success(const Value& v)
        {
            Result r;
            r.ok = true;
            r.value = v;
            return r;
        }

        //!\brief Create error result
        static Result error(const std::string& code, const std::string& message)
        {
            Result r;
            r.ok = false;
            r.error_code = code;
            r.error_message = message;
            return r;
        }
    };

} // namespace OPERON

#endif // OPERON_RESULT_HPP