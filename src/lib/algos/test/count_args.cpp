// -------------------------------------------------------------
//
//!\file count_args.cpp
//!\brief Test count_args algorithm implementation
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "count_args.hpp"

namespace OPERON::Algos::Test {

    //
    //!\brief Create the test.count_args algorithm
    //
    Algorithm make_count_args_algorithm()
    {
        Algorithm count_args;

        count_args.info.name = "test.count_args";
        count_args.info.category = "test";
        count_args.info.brief = "Return the number of supplied arguments";
        count_args.info.return_type = ValueType::Int;
        count_args.function = [](Context&, const std::vector<Value>& args) -> Result
        {
            return Result::success(OPERON::Value(static_cast<long long>(args.size())));
        };

        return count_args;
    }

} // namespace OPERON::Algos::Test
