// -------------------------------------------------------------
//
//!\file main.cpp
//!\brief Console client for interacting with the Operon runtime
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
#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

//
//!\brief Trim leading and trailing whitespace from a string
//
static std::string trim(const std::string& value)
{
    size_t start = 0;
    size_t end = value.size();

    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start])))
    {
        ++start;
    }

    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
    {
        --end;
    }

    return value.substr(start, end - start);
}

//
//!\brief Convert value type to string
//
static std::string type_to_string(OPERON::ValueType type)
{
    switch (type)
    {
        case OPERON::ValueType::Null:
            return "null";

        case OPERON::ValueType::Bool:
            return "bool";

        case OPERON::ValueType::Int:
            return "int";

        case OPERON::ValueType::Float:
            return "float";

        case OPERON::ValueType::String:
            return "string";

        case OPERON::ValueType::Buffer:
            return "buffer";

        case OPERON::ValueType::Array:
            return "array";

        default:
            return "unknown";
    }
}

//
//!\brief Read a full line from standard input
//
static std::string read_line(const std::string& prompt)
{
    std::string line;

    std::cout << prompt;
    std::getline(std::cin, line);

    return line;
}

//
//!\brief Split a command line into tokens with basic quote support
//
static std::vector<std::string> split_command_line(const std::string& line)
{
    std::vector<std::string> tokens;
    std::string current;
    bool in_quotes = false;
    char quote_char = '\0';

    for (size_t i = 0; i < line.size(); ++i)
    {
        const char c = line[i];

        if (in_quotes)
        {
            if (c == '\\' && i + 1 < line.size())
            {
                current += line[i + 1];
                ++i;
                continue;
            }

            if (c == quote_char)
            {
                in_quotes = false;
                quote_char = '\0';
                continue;
            }

            current += c;
            continue;
        }

        if (c == '"' || c == '\'')
        {
            in_quotes = true;
            quote_char = c;
            continue;
        }

        if (std::isspace(static_cast<unsigned char>(c)))
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }

            continue;
        }

        current += c;
    }

    if (!current.empty())
    {
        tokens.push_back(current);
    }

    return tokens;
}

//
//!\brief Print command help
//
static void print_help()
{
    std::cout << "Operon CLI" << std::endl;
    std::cout << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  help" << std::endl;
    std::cout << "  list" << std::endl;
    std::cout << "  info <algorithm>" << std::endl;
    std::cout << "  run <algorithm> [args...]" << std::endl;
    std::cout << "  exit" << std::endl;
    std::cout << "  quit" << std::endl;
    std::cout << std::endl;
}

//
//!\brief Print all algorithms
//
static void cmd_list(OPERON::Operon& operon)
{
    const std::vector<std::string> algorithms = operon.list_algorithms();

    if (algorithms.empty())
    {
        std::cout << "No algorithms are currently registered." << std::endl;
        return;
    }

    for (const std::string& name : algorithms)
    {
        std::cout << name << std::endl;
    }
}

//
//!\brief Print algorithm metadata
//
static void cmd_info(OPERON::Operon& operon, const std::string& algorithm_name)
{
    OPERON::AlgorithmInfo info;

    if (!operon.get_algorithm_info(algorithm_name, info))
    {
        std::cout << "Algorithm not found." << std::endl;
        return;
    }

    std::cout << "Name: " << info.name << std::endl;
    std::cout << "Category: " << info.category << std::endl;
    std::cout << "Brief: " << info.brief << std::endl;
    std::cout << "Return type: " << type_to_string(info.return_type) << std::endl;

    if (info.parameters.empty())
    {
        std::cout << "Parameters: none" << std::endl;
        return;
    }

    std::cout << "Parameters:" << std::endl;

    for (const auto& param : info.parameters)
    {
        std::cout << "  " << param.name
                  << " (" << type_to_string(param.type) << ")";

        if (!param.required)
        {
            std::cout << " optional";
        }

        if (!param.brief.empty())
        {
            std::cout << " - " << param.brief;
        }

        std::cout << std::endl;
    }
}

//
//!\brief Parse a string into the requested value type
//
static OPERON::Value parse_typed_value(OPERON::ValueType type, const std::string& raw)
{
    const std::string value = trim(raw);

    switch (type)
    {
        case OPERON::ValueType::Null:
            return OPERON::Value();

        case OPERON::ValueType::Bool:
        {
            if (value == "true" || value == "1" || value == "yes")
            {
                return OPERON::Value(true);
            }

            if (value == "false" || value == "0" || value == "no")
            {
                return OPERON::Value(false);
            }

            throw std::runtime_error("Expected bool value");
        }

        case OPERON::ValueType::Int:
            return OPERON::Value(std::stoll(value));

        case OPERON::ValueType::Float:
            return OPERON::Value(std::stod(value));

        case OPERON::ValueType::String:
            return OPERON::Value(value);

        case OPERON::ValueType::Buffer:
        {
            OPERON::Value::Buffer buffer(value.begin(), value.end());
            return OPERON::Value(buffer);
        }

        case OPERON::ValueType::Array:
            throw std::runtime_error("Array input is not yet supported");

        default:
            throw std::runtime_error("Unsupported value type");
    }
}

//
//!\brief Convert an Operon value to a printable string
//
static std::string value_to_string(const OPERON::Value& value)
{
    switch (value.type())
    {
        case OPERON::ValueType::Null:
            return "null";

        case OPERON::ValueType::Bool:
            return value.as_bool() ? "true" : "false";

        case OPERON::ValueType::Int:
            return std::to_string(value.as_int());

        case OPERON::ValueType::Float:
            return std::to_string(value.as_float());

        case OPERON::ValueType::String:
            return value.as_string();

        case OPERON::ValueType::Buffer:
        {
            const OPERON::Value::Buffer& buffer = value.as_buffer();
            return std::string(buffer.begin(), buffer.end());
        }

        case OPERON::ValueType::Array:
        {
            const OPERON::Value::Array& array = value.as_array();
            std::string result = "[";

            for (size_t i = 0; i < array.size(); ++i)
            {
                if (i > 0)
                {
                    result += ", ";
                }

                result += value_to_string(array[i]);
            }

            result += "]";
            return result;
        }

        default:
            return "<unknown>";
    }
}

//
//!\brief Prompt for any missing arguments using algorithm metadata
//
static bool prompt_missing_args(const OPERON::AlgorithmInfo& info, std::vector<OPERON::Value>& args)
{
    for (size_t i = args.size(); i < info.parameters.size(); ++i)
    {
        const auto& param = info.parameters[i];

        while (true)
        {
            try
            {
                const std::string raw = read_line(param.name + " (" + type_to_string(param.type) + "): ");

                if (!param.required && trim(raw).empty())
                {
                    break;
                }

                args.push_back(parse_typed_value(param.type, raw));
                break;
            }
            catch (const std::exception& ex)
            {
                std::cout << "Invalid value: " << ex.what() << std::endl;
            }
        }
    }

    return true;
}

//
//!\brief Execute an algorithm using command arguments and prompts for missing values
//
static void cmd_run(OPERON::Operon& operon, const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2)
    {
        std::cout << "Missing algorithm name." << std::endl;
        return;
    }

    const std::string& algorithm_name = tokens[1];
    OPERON::AlgorithmInfo info;

    if (!operon.get_algorithm_info(algorithm_name, info))
    {
        std::cout << "Algorithm not found." << std::endl;
        return;
    }

    std::vector<OPERON::Value> args;

    for (size_t i = 0; i < info.parameters.size() && (i + 2) < tokens.size(); ++i)
    {
        try
        {
            args.push_back(parse_typed_value(info.parameters[i].type, tokens[i + 2]));
        }
        catch (const std::exception& ex)
        {
            std::cout << "Invalid value for parameter '" << info.parameters[i].name
                      << "': " << ex.what() << std::endl;
            return;
        }
    }

    if (args.size() < info.parameters.size())
    {
        if (!prompt_missing_args(info, args))
        {
            return;
        }
    }

    const OPERON::Result result = operon.execute(algorithm_name, args);

    if (!result.ok)
    {
        std::cout << "Error: " << result.error_code << ": " << result.error_message << std::endl;
        return;
    }

    std::cout << value_to_string(result.value) << std::endl;
}

//
//!\brief Execute a command and return whether the shell should continue
//
static bool run_command(OPERON::Operon& operon, const std::vector<std::string>& tokens)
{
    if (tokens.empty())
    {
        return true;
    }

    const std::string& command = tokens[0];

    if (command == "help")
    {
        print_help();
        return true;
    }

    if (command == "list")
    {
        cmd_list(operon);
        return true;
    }

    if (command == "info")
    {
        if (tokens.size() < 2)
        {
            std::cout << "Missing algorithm name." << std::endl;
            return true;
        }

        cmd_info(operon, tokens[1]);
        return true;
    }

    if (command == "run")
    {
        cmd_run(operon, tokens);
        return true;
    }

    if (command == "exit" || command == "quit")
    {
        return false;
    }

    std::cout << "Unknown command. Type 'help' for usage." << std::endl;
    return true;
}

//
//!\brief Run the interactive Operon shell
//
static int run_repl(OPERON::Operon& operon)
{
    std::cout << "Operon interactive shell" << std::endl;
    std::cout << "Type 'help' for usage, 'exit' to quit." << std::endl;

    while (true)
    {
        std::string line = read_line("operon> ");
        line = trim(line);

        if (line.empty())
        {
            continue;
        }

        const std::vector<std::string> tokens = split_command_line(line);

        if (!run_command(operon, tokens))
        {
            break;
        }
    }

    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    OPERON::Operon operon;

    if (!operon.register_builtins())
    {
        std::cout << "Warning: failed to register built-in algorithms." << std::endl;
    }

    if (argc <= 1)
    {
        return run_repl(operon);
    }

    std::vector<std::string> tokens;

    for (int i = 1; i < argc; ++i)
    {
        tokens.push_back(argv[i]);
    }

    run_command(operon, tokens);
    return EXIT_SUCCESS;
}