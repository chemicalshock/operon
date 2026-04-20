// -------------------------------------------------------------
//
//!\file echo.cpp
//!\brief Test echo algorithm implementation
//!\author Colin J.D. Stewart
//
// -------------------------------------------------------------
//
//           Copyright (c) 2026. Colin J.D. Stewart.
//                   All rights reserved
//
// -------------------------------------------------------------

// own header
#include "echo.hpp"

namespace OPERON::Algos::Test {

    //
    //!\brief Create the test.echo algorithm
    //
    Algorithm make_echo_algorithm()
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

        return echo;
    }

} // namespace OPERON::Algos::Test
