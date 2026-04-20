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
//!\brief Read an integer value from standard input
//
static int read_int(const std::string& prompt)
{
    while (true)
    {
        const std::string line = trim(read_line(prompt));

        try
        {
            size_t consumed = 0;
            const int value = std::stoi(line, &consumed);

            if (consumed == line.size())
            {
                return value;
            }
        }
        catch (const std::exception&)
        {
        }

        std::cout << "Invalid integer value." << std::endl;
    }
}

//
//!\brief Convert a value type to printable text
//
static std::string value_type_to_string(OPERON::ValueType type)
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
//!\brief Parse a text input into the requested value type
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
            throw std::runtime_error("Array input is not yet supported in the console");

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
//!\brief Print the main menu
//
static void print_menu()
{
    std::cout << std::endl;
    std::cout << "Operon Console" << std::endl;
    std::cout << "1. List algorithms" << std::endl;
    std::cout << "2. Show algorithm info" << std::endl;
    std::cout << "3. Execute algorithm" << std::endl;
    std::cout << "4. Quit" << std::endl;
    std::cout << std::endl;
}

//
//!\brief Print all available algorithms
//
static void print_algorithms(OPERON::Operon& operon)
{
    const std::vector<std::string> algorithms = operon.list_algorithms();

    if (algorithms.empty())
    {
        std::cout << "No algorithms are currently registered." << std::endl;
        return;
    }

    std::cout << "Available algorithms:" << std::endl;

    for (const std::string& name : algorithms)
    {
        std::cout << " - " << name << std::endl;
    }
}

//
//!\brief Print algorithm metadata
//
static void print_algorithm_info(OPERON::Operon& operon)
{
    const std::string algorithm_name = trim(read_line("Algorithm name: "));
    OPERON::AlgorithmInfo info;

    if (!operon.get_algorithm_info(algorithm_name, info))
    {
        std::cout << "Algorithm not found." << std::endl;
        return;
    }

    std::cout << "Name: " << info.name << std::endl;
    std::cout << "Category: " << info.category << std::endl;
    std::cout << "Brief: " << info.brief << std::endl;
    std::cout << "Return type: " << value_type_to_string(info.return_type) << std::endl;

    if (info.parameters.empty())
    {
        std::cout << "Parameters: none" << std::endl;
        return;
    }

    std::cout << "Parameters:" << std::endl;

    for (const auto& param : info.parameters)
    {
        std::cout << " - " << param.name
                  << " (" << value_type_to_string(param.type) << ")";

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
//!\brief Execute an algorithm interactively using metadata prompts
//
static void execute_algorithm(OPERON::Operon& operon)
{
    const std::string algorithm_name = trim(read_line("Algorithm name: "));
    OPERON::AlgorithmInfo info;

    if (!operon.get_algorithm_info(algorithm_name, info))
    {
        std::cout << "Algorithm not found." << std::endl;
        return;
    }

    std::vector<OPERON::Value> args;
    args.reserve(info.parameters.size());

    for (const auto& param : info.parameters)
    {
        while (true)
        {
            try
            {
                std::string prompt = param.name + " (" + value_type_to_string(param.type) + "): ";
                const std::string raw = read_line(prompt);

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

    const OPERON::Result result = operon.execute(algorithm_name, args);

    if (!result.ok)
    {
        std::cout << "Execution failed." << std::endl;
        std::cout << "Code: " << result.error_code << std::endl;
        std::cout << "Message: " << result.error_message << std::endl;
        return;
    }

    std::cout << "Result: " << value_to_string(result.value) << std::endl;
}

int main()
{
    OPERON::Operon operon;

    if (!operon.register_builtins())
    {
        std::cout << "Warning: failed to register built-in algorithms." << std::endl;
    }

    while (true)
    {
        print_menu();

        const int choice = read_int("Select option: ");

        switch (choice)
        {
            case 1:
                print_algorithms(operon);
                break;

            case 2:
                print_algorithm_info(operon);
                break;

            case 3:
                execute_algorithm(operon);
                break;

            case 4:
                std::cout << "Exiting Operon console." << std::endl;
                return EXIT_SUCCESS;

            default:
                std::cout << "Unknown option." << std::endl;
                break;
        }
    }
}