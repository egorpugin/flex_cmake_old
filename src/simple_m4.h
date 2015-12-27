#pragma once

#include <map>
#include <string>

#include "context.h"

namespace simple_m4
{

enum class State
{
    None,
    Macro,
};

enum class MacroType
{
    None,
    UserDefined,
    dnl,
    changecom,
    changequote,
    define,
    ifdef,
    ifelse,
    undefine,
    include,
};

struct MacroParam
{
    std::string text;
    bool quoted;
};

using MacroParams = std::vector<MacroParam>;

struct Macro
{
    MacroType type;
    MacroParams macro_params;
};

using M4Functions = std::map<std::string, MacroType>;

class M4
{
public:
    M4();

    std::string processLine(const std::string &text, const MacroParams &params = MacroParams());

    void setFunctions(const M4Functions &functions)
    {
        this->functions = functions;
    }

    void addDefinition(const std::string &key, const std::string &value)
    {
        definitions[key] = value;
    }

private:
    State state = State::None;
    std::map<std::string, std::string> definitions;
    std::string open_quote;
    std::string closing_quote;
    std::string comment;
    int n_quotes;
    std::stack<Macro> macros;
    MacroParam param;
    MacroType lastMacro;
    M4Functions functions;
};

std::string read_file(const std::string &filename);

} // namespace simple_m4

bool m4(Context::Lines &lines);
