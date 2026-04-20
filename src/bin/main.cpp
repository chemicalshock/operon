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
#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <termios.h>
#include <unistd.h>

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

struct CompletionResult
{
    size_t replace_start = 0;
    std::vector<std::string> matches;
    bool append_space = true;
};

struct ReadLineSession
{
    std::vector<std::string> history;
};

using CompletionProvider = std::function<CompletionResult(const std::string&, size_t)>;

class RawTerminalMode
{
public:
    RawTerminalMode()
    {
        if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &m_original) != 0)
        {
            return;
        }

        termios raw = m_original;
        raw.c_lflag &= static_cast<tcflag_t>(~(ECHO | ICANON));
        raw.c_iflag &= static_cast<tcflag_t>(~IXON);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0)
        {
            m_enabled = true;
        }
    }

    ~RawTerminalMode()
    {
        if (m_enabled)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &m_original);
        }
    }

    bool enabled() const
    {
        return m_enabled;
    }

private:
    termios m_original{};
    bool m_enabled = false;
};

//
//!\brief Read one byte from standard input
//
static bool read_input_char(char& c)
{
    while (true)
    {
        const ssize_t bytes_read = read(STDIN_FILENO, &c, 1);

        if (bytes_read == 1)
        {
            return true;
        }

        if (bytes_read == 0)
        {
            return false;
        }

        if (errno != EINTR)
        {
            return false;
        }
    }
}

//
//!\brief Redraw the editable command line
//
static void redraw_line(const std::string& prompt, const std::string& line, size_t cursor)
{
    std::cout << '\r' << prompt << line << "\x1b[K";

    const size_t chars_right = line.size() - cursor;

    if (chars_right > 0)
    {
        std::cout << "\x1b[" << chars_right << "D";
    }

    std::cout.flush();
}

//
//!\brief Return whether value starts with prefix
//
static bool starts_with(const std::string& value, const std::string& prefix)
{
    return value.size() >= prefix.size()
        && value.compare(0, prefix.size(), prefix) == 0;
}

//
//!\brief Return the shared prefix for a list of matches
//
static std::string common_prefix(const std::vector<std::string>& values)
{
    if (values.empty())
    {
        return "";
    }

    std::string prefix = values[0];

    for (size_t i = 1; i < values.size(); ++i)
    {
        size_t count = 0;
        const size_t limit = std::min(prefix.size(), values[i].size());

        while (count < limit && prefix[count] == values[i][count])
        {
            ++count;
        }

        prefix.resize(count);
    }

    return prefix;
}

//
//!\brief Find the start of the token at the cursor
//
static size_t current_token_start(const std::string& line, size_t cursor)
{
    size_t start = cursor;

    while (start > 0 && !std::isspace(static_cast<unsigned char>(line[start - 1])))
    {
        --start;
    }

    return start;
}

//
//!\brief Apply one tab completion attempt
//
static void apply_completion(
    const std::string& prompt,
    std::string& line,
    size_t& cursor,
    const CompletionProvider& completion)
{
    if (!completion)
    {
        std::cout << '\a' << std::flush;
        return;
    }

    CompletionResult result = completion(line, cursor);

    if (result.matches.empty() || result.replace_start > cursor)
    {
        std::cout << '\a' << std::flush;
        return;
    }

    std::sort(result.matches.begin(), result.matches.end());
    result.matches.erase(
        std::unique(result.matches.begin(), result.matches.end()),
        result.matches.end());

    const std::string prefix = line.substr(result.replace_start, cursor - result.replace_start);
    const std::string replacement = common_prefix(result.matches);

    if (replacement.size() > prefix.size() || result.matches.size() == 1)
    {
        line.replace(result.replace_start, cursor - result.replace_start, replacement);
        cursor = result.replace_start + replacement.size();

        if (result.matches.size() == 1 && result.append_space)
        {
            if (cursor == line.size() || !std::isspace(static_cast<unsigned char>(line[cursor])))
            {
                line.insert(cursor, 1, ' ');
                ++cursor;
            }
        }

        redraw_line(prompt, line, cursor);
        return;
    }

    std::cout << "\r\n";

    for (const std::string& match : result.matches)
    {
        std::cout << match << "\r\n";
    }

    redraw_line(prompt, line, cursor);
}

//
//!\brief Read a full line from standard input
//
static bool read_line(const std::string& prompt, std::string& line)
{
    std::cout << prompt;
    return static_cast<bool>(std::getline(std::cin, line));
}

//
//!\brief Read a full editable line from standard input
//
static bool read_line(
    const std::string& prompt,
    std::string& line,
    ReadLineSession& session,
    const CompletionProvider& completion)
{
    line.clear();

    if (!isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
    {
        return read_line(prompt, line);
    }

    RawTerminalMode raw_mode;

    if (!raw_mode.enabled())
    {
        return read_line(prompt, line);
    }

    std::cout << prompt << std::flush;

    size_t cursor = 0;
    size_t history_index = session.history.size();
    std::string draft;

    while (true)
    {
        char c = '\0';

        if (!read_input_char(c))
        {
            std::cout << "\r\n";
            return false;
        }

        if (c == '\r' || c == '\n')
        {
            std::cout << "\r\n";

            if (!trim(line).empty() && (session.history.empty() || session.history.back() != line))
            {
                session.history.push_back(line);
            }

            return true;
        }

        if (c == '\t')
        {
            apply_completion(prompt, line, cursor, completion);
            history_index = session.history.size();
            draft = line;
            continue;
        }

        if (c == 4)
        {
            if (line.empty())
            {
                std::cout << "\r\n";
                return false;
            }

            if (cursor < line.size())
            {
                line.erase(cursor, 1);
                history_index = session.history.size();
                draft = line;
                redraw_line(prompt, line, cursor);
            }

            continue;
        }

        if (c == 127 || c == '\b')
        {
            if (cursor > 0)
            {
                line.erase(cursor - 1, 1);
                --cursor;
                history_index = session.history.size();
                draft = line;
                redraw_line(prompt, line, cursor);
            }

            continue;
        }

        if (c == 27)
        {
            char seq1 = '\0';
            char seq2 = '\0';

            if (!read_input_char(seq1))
            {
                continue;
            }

            if (seq1 != '[' && seq1 != 'O')
            {
                continue;
            }

            if (!read_input_char(seq2))
            {
                continue;
            }

            if (seq2 == 'A')
            {
                if (!session.history.empty() && history_index > 0)
                {
                    if (history_index == session.history.size())
                    {
                        draft = line;
                    }

                    --history_index;
                    line = session.history[history_index];
                    cursor = line.size();
                    redraw_line(prompt, line, cursor);
                }

                continue;
            }

            if (seq2 == 'B')
            {
                if (history_index < session.history.size())
                {
                    ++history_index;
                    line = (history_index == session.history.size()) ? draft : session.history[history_index];
                    cursor = line.size();
                    redraw_line(prompt, line, cursor);
                }

                continue;
            }

            if (seq2 == 'C')
            {
                if (cursor < line.size())
                {
                    ++cursor;
                    redraw_line(prompt, line, cursor);
                }

                continue;
            }

            if (seq2 == 'D')
            {
                if (cursor > 0)
                {
                    --cursor;
                    redraw_line(prompt, line, cursor);
                }

                continue;
            }

            if (seq2 == 'H')
            {
                cursor = 0;
                redraw_line(prompt, line, cursor);
                continue;
            }

            if (seq2 == 'F')
            {
                cursor = line.size();
                redraw_line(prompt, line, cursor);
                continue;
            }

            if (seq2 == '3')
            {
                char seq3 = '\0';

                if (read_input_char(seq3) && seq3 == '~' && cursor < line.size())
                {
                    line.erase(cursor, 1);
                    history_index = session.history.size();
                    draft = line;
                    redraw_line(prompt, line, cursor);
                }
            }

            continue;
        }

        if (std::isprint(static_cast<unsigned char>(c)))
        {
            line.insert(cursor, 1, c);
            ++cursor;
            redraw_line(prompt, line, cursor);
            history_index = session.history.size();
            draft = line;
        }
    }
}

//
//!\brief Return the known shell command names
//
static const std::vector<std::string>& command_names()
{
    static const std::vector<std::string> names = {
        "exit",
        "help",
        "info",
        "list",
        "quit",
        "run"
    };

    return names;
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
//!\brief Complete shell commands and registered algorithm names
//
static CompletionResult complete_command_line(
    const OPERON::Operon& operon,
    const std::string& line,
    size_t cursor)
{
    CompletionResult result;
    result.replace_start = current_token_start(line, cursor);

    const std::string prefix = line.substr(result.replace_start, cursor - result.replace_start);
    const std::vector<std::string> tokens_before =
        split_command_line(line.substr(0, result.replace_start));
    const size_t token_index = tokens_before.size();

    std::vector<std::string> candidates;

    if (token_index == 0)
    {
        candidates = command_names();
    }
    else if (token_index == 1 && (tokens_before[0] == "info" || tokens_before[0] == "run"))
    {
        candidates = operon.list_algorithms();
    }

    for (const std::string& candidate : candidates)
    {
        if (starts_with(candidate, prefix))
        {
            result.matches.push_back(candidate);
        }
    }

    return result;
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
                std::string raw;

                if (!read_line(param.name + " (" + type_to_string(param.type) + "): ", raw))
                {
                    return false;
                }

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

    ReadLineSession input_session;
    const CompletionProvider completion =
        [&operon](const std::string& line, size_t cursor) -> CompletionResult
        {
            return complete_command_line(operon, line, cursor);
        };

    while (true)
    {
        std::string line;

        if (!read_line("operon> ", line, input_session, completion))
        {
            break;
        }

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
