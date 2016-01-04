#pragma once

#include <map>
#include <string>

#include "context.h"

namespace simple_m4
{

enum class State
{
    None,
    GatherMacroParameters,
    MacroCall,
    NoMacroCalls,
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

using MacroParam = std::string;
using MacroParams = std::vector<MacroParam>;

struct Macro
{
    MacroType type;
    MacroParams macro_params;
};

enum class TokenType
{
    eos,
    eol,
    word,
    digit_word,
    open_quote,
    closing_quote,
    integer,
    comment,
    character,
    open_brace,
    closing_brace,
    comma,
    dollar,
};

struct M4Token
{
    TokenType type;
    std::string word;
    int length;
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
    std::string getDefinition(const std::string &key)
    {
        return definitions[key];
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
    M4Functions functions;
    int n_brackets;

    const char *p; // current input symbol
    M4Token token;
    std::string quouted_string;

    M4Token getNextToken();
    M4Token getNextToken1();
    void backtrackToken();
    void skipToNewLine();
    void readToCommaOrRBracket(const MacroParams &params);
    std::string readToClosingQuote(const MacroParams &params);
    std::string processDollar(const MacroParams &params);
    bool isQuoted(const std::string &s);
    bool removeQuotes(std::string &s);
    bool isEmpty(const std::string &s);

    std::string processWord();
    void processParameter();
    std::string processMacro(const MacroParams &params);
};

std::string read_file(const std::string &filename);

} // namespace simple_m4

bool m4(Context::Lines &lines);
