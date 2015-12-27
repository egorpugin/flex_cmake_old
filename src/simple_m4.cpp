#include "simple_m4.h"

#include <assert.h>
#include <fstream>
#include <map>
#include <set>
#include <stack>
#include <sstream>

#define DEFAULT_OPEN_QUOTE "`"
#define DEFAULT_CLOSE_QUOTE "'"

#define DEFAULT_COMMENT "#"

namespace simple_m4
{

const M4Functions main_functions =
{
    { "m4_dnl",MacroType::dnl },
    { "m4_changecom",MacroType::changecom },
    { "m4_changequote",MacroType::changequote },
    { "m4_define",MacroType::define },
    { "m4_ifdef",MacroType::ifdef },
    { "m4_ifelse",MacroType::ifelse },
    { "m4_undefine",MacroType::undefine },
};

M4::M4()
{
    open_quote = DEFAULT_OPEN_QUOTE;
    closing_quote = DEFAULT_CLOSE_QUOTE;
    comment = DEFAULT_COMMENT;
    n_quotes = 0;
}

std::string M4::processLine(const std::string &text, const MacroParams &params)
{
    std::string s;
    for (auto i = text.begin(); i != text.end(); ++i)
    {
        bool eol = !*(&*i + 1);
        auto c = *i;

        if (!comment.empty() && memcmp(&*i, comment.c_str(), comment.size()) == 0)
        {
            s += &*i;
            break;
        }

        if (memcmp(&*i, open_quote.c_str(), open_quote.size()) == 0)
        {
            for (int s = 1; s < open_quote.size(); s++)
                ++i;
            eol = !*(&*i + 1);
            n_quotes++;
            if (macros.size())
            {
                if (n_quotes > 1)
                {
                    param.text += open_quote;
                }
                if (eol)
                {
                    param.text += '\n';
                }
            }
            continue;
        }
        else if (n_quotes > 0 && memcmp(&*i, closing_quote.c_str(), closing_quote.size()) == 0)
        {
            for (int s = 1; s < closing_quote.size(); s++)
                ++i;
            eol = !*(&*i + 1);
            n_quotes--;
            if (n_quotes > 0 && macros.size())
            {
                param.text += closing_quote;
                if (eol)
                {
                    param.text += '\n';
                }
            }
            continue;
        }

        switch (state)
        {
        case State::None:
        {
            std::string word;
            bool last_sym_added = false;
            if (isalnum(c) || c == '_')
            {
                do
                {
                    c = *i;
                    eol = !*(&*i + 1);
                    if (isalnum(c) || c == '_')
                    {
                        word += c;
                        last_sym_added = true;
                    }
                    else
                    {
                        last_sym_added = false;
                        break;
                    }
                    i++;
                } while (i != text.end());
            }
            else if (c == '$' && !params.empty())
            {
                ++i;
                eol = !*(&*i + 1);
                c = *i;
                if (isdigit(c))
                {
                    s += processLine(params[c - '0'].text);
                }
                else
                {
                    switch (c)
                    {
                    case '*':
                    {
                        int i = 0;
                        for (auto &p : params)
                        {
                            if (i++)
                                s += processLine(p.text) + ",";
                        }
                        s.resize(s.size() - 1);
                    }
                        break;
                    case '#':
                    case '@':
                        assert(false);
                        break;
                    default:
                        s += '$';
                        --i;
                        break;
                    }
                }
                break;
            }
            else if (word.empty())
            {
                s += c;
                break;
            }

            {
                auto m = functions.find(word);
                if (m == functions.end() || n_quotes > 0)
                {
                    if (isalpha(word[0]) || word[0] == '_')
                    {
                        auto d = definitions.find(word);
                        if (d != definitions.end() && n_quotes == 0)
                        {
                            // no brace, simple replacement
                            if (c != '(')
                            {
                                word = processLine(d->second);
                            }
                            else
                            {
                                state = State::Macro;
                                macros.push(Macro{ MacroType::UserDefined });
                                macros.top().macro_params.push_back(MacroParam{ word });
                                continue;
                            }
                        }
                    }

                    s += word;
                    if (eol || !last_sym_added)
                        --i;
                    continue;
                }

                // dnl comes w/out braces
                if (m->second == MacroType::dnl)
                {
                    lastMacro = MacroType::dnl;
                    while (i != text.end())
                    {
                        if (*i == '\n')
                        {
                            i++;
                            break;
                        }
                        i++;
                    }
                    if (i == text.end())
                        --i;
                    else if (eol)
                        --i;
                    break;
                }

                // no brace, no output
                if (c != '(')
                {
                    if (m->second == MacroType::changequote)
                    {
                        open_quote = DEFAULT_OPEN_QUOTE;
                        closing_quote = DEFAULT_CLOSE_QUOTE;
                    }
                    else if (m->second == MacroType::changecom)
                    {
                        comment = "";
                    }

                    --i;
                    continue;
                }

                state = State::Macro;
                macros.push(Macro{ m->second });
                macros.top().macro_params.push_back(MacroParam{ m->first });
            }
        }
        break;
        case State::Macro:
        {
            std::string word;
            bool last_sym_added = false;
            if (n_quotes == 0 && (isalnum(c) || c == '_'))
            {
                do
                {
                    c = *i;
                    eol = !*(&*i + 1);
                    if (isalnum(c) || c == '_')
                    {
                        word += c;
                        last_sym_added = true;
                    }
                    else
                    {
                        last_sym_added = false;
                        break;
                    }
                    i++;
                } while (i != text.end());
            }

            if (n_quotes)
            {
                param.quoted = true;
                param.text += c;
                if (eol)
                    param.text += '\n';
                break;
            }
            else if (isspace(c) && param.text.empty())
            {
                param.text += word;
                if (!last_sym_added && !param.text.empty())
                    param.text += c;
                break;
            }
            else if (n_quotes == 0 && (c == ',' || c == ')'))
            {
                param.text += word;

                // trim begin, end
                if (param.quoted)
                {
                    // begin
                    int s = 0;
                    for (auto r = param.text.begin(); r != param.text.end(); ++r)
                    {
                        char c = *r;
                        if (isspace(c))
                            s++;
                        else
                            break;
                    }
                    if (s)
                        param.text = param.text.substr(s);
                    // end
                    s = 0;
                    for (auto r = param.text.rbegin(); r != param.text.rend(); ++r)
                    {
                        char c = *r;
                        if (isspace(c))
                            s++;
                        else
                            break;
                    }
                    if (s)
                        param.text.resize(param.text.size() - s);
                }

                auto &macro = macros.top();

                // add
                if (!(macro.macro_params.size() == 1 && c == ')' && param.text.empty()))
                    macro.macro_params.push_back(param);
                param = MacroParam{};

                if (c == ',')
                {
                    if (eol)
                        param.text += '\n';
                    break;
                }

                state = State::None;

                // exec macro
                switch (macro.type)
                {
                case MacroType::changequote:
                    switch (macro.macro_params.size())
                    {
                    case 1:
                        open_quote = DEFAULT_OPEN_QUOTE;
                        closing_quote = DEFAULT_CLOSE_QUOTE;
                        break;
                    case 2:
                        open_quote = macro.macro_params[1].text;
                        closing_quote = DEFAULT_CLOSE_QUOTE;
                        break;
                    case 3:
                        open_quote = macro.macro_params[1].text;
                        closing_quote = macro.macro_params[2].text;
                        break;
                    }
                    break;
                case MacroType::define:
                    switch (macro.macro_params.size())
                    {
                    case 2:
                        definitions[macro.macro_params[1].text];
                        break;
                    case 3:
                        definitions[macro.macro_params[1].text] = macro.macro_params[2].text;
                        break;
                    }
                    break;
                case MacroType::ifdef:
                {
                    auto d = definitions.find(macro.macro_params[1].text);
                    if (d != definitions.end())
                    {
                        s += processLine(macro.macro_params[2].text);
                    }
                    else if (macro.macro_params.size() > 3)
                    {
                        s += processLine(macro.macro_params[3].text);
                    }
                }
                    break;
                case MacroType::ifelse:
                    switch (macro.macro_params.size())
                    {
                    case 5:
                    {
                        auto s1 = processLine(macro.macro_params[1].text);
                        auto s2 = processLine(macro.macro_params[2].text);
                        if (s1 == s2)
                            s += processLine(macro.macro_params[3].text);
                        else
                            s += processLine(macro.macro_params[4].text);
                    }
                        break;
                    default:
                        assert(false);
                        break;
                    }
                    break;
                case MacroType::UserDefined:
                {
                    auto d = definitions[macro.macro_params[0].text];
                    s += processLine(d, macro.macro_params);
                    if (lastMacro == MacroType::dnl)
                    {
                        while (i != text.end())
                        {
                            if (*i == '\n')
                            {
                                break;
                            }
                            i++;
                        }
                        if (i == text.end())
                            --i;
                    }
                }
                    break;
                case MacroType::undefine:
                    definitions.erase(macro.macro_params[1].text);
                    break;
                case MacroType::include:
                {
                    auto f = read_file(macro.macro_params[1].text);
                    s += f;
                }
                    break;
                default:
                    assert(false);
                    break;
                }
                lastMacro = macro.type;

                macros.pop();
            }
            else
            {
                if (!word.empty())
                {
                    param.text += word;
                }
                if (!last_sym_added)
                {
                    param.text += c;
                }
                if (eol)
                    param.text += '\n';
            }
        }
        break;
        default:
            assert(false);
            break;
        }
    }
    if (text.empty() && state == State::Macro)
        param.text += '\n';
    return s;
}

std::string read_file(const std::string &filename)
{
    std::ifstream ifile(filename);
    if (!ifile)
        throw std::runtime_error("Cannot open file " + filename);
    std::string f, s;
    while (std::getline(ifile, s))
        f += s + "\n";
    return f;
}

} // namespace simple_m4

bool m4(Context::Lines &lines)
{
    simple_m4::M4 processor;
    processor.setFunctions(simple_m4::main_functions);
    for (auto &line : lines)
        line.text = processor.processLine(line.text);
    return true;
}
