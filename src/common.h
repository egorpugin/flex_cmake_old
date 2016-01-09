#pragma once

#include <set>
#include <stdint.h>
#include <string>
#include <queue>
#include <unordered_set>
#include <vector>

using Character = uint32_t;

using String = std::string;
using Strings = std::vector<String>;

using InputFiles = std::queue<String>;

// rules
enum class RuleType
{
    Normal = 0,
    Variable = 1,
};

struct Rule
{
    RuleType type = RuleType::Normal;
    int linenum = 0;
    bool useful = false;
    bool has_nl = false;
};

using Rules = std::vector<Rule>;

// start conditions
struct StartCondition
{
    String name;
    int set; // set of rules active in start condition
    int bol; // set of rules active only at the beginning of line in a s.c.
    bool xclu; // true if start condition is exclusive
    bool eof; // true if start condition has EOF rule
};

using StartConditions = std::vector<StartCondition>;

// character classes
using CharacterTable = std::set<Character>;

struct CharacterClass
{
    using Table = CharacterTable;

    Table table;   // holds the characters in each ccl
    bool ng = false;        // true for a given ccl if the ccl is negated
    bool has_nl = false;    // true if current ccl could match a newline
};

using CharacterClasses = std::vector<CharacterClass>;
