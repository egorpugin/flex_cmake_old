// new file

#pragma once

#include <list>
#include <memory>
#include <string>
#include <stack>
#include <sstream>
#include <vector>

class Context
{
public:
    using Text = std::string;

    static struct eol_type {} eol;

public:
    struct Line
    {
        Text text;
        int n_indents = 0;

        Line() {}
        Line(const Text &t, int n = 0)
            : text(t), n_indents(n)
        {}

        Line& operator+=(const Text &t)
        {
            text += t;
            return *this;
        }
    };

    using Lines = std::list<Line>;

public:
    Context(const Text &indent = "    ", const Text &newline = "\n");

    void initFromString(const std::string &s);

    void addLine(const Text &s = Text());
    void addNoNewLine(const Text &s);
    void addLineNoSpace(const Text &s);

    void addText(const Text &s);
    void addText(const char* str, int n);

    void decreaseIndent();
    void increaseIndent();

    void beginBlock(const Text &s = "", bool indent = true);
    void endBlock(bool semicolon = false);
    void beginFunction(const Text &s = "");
    void endFunction();
    void beginNamespace(const Text &s);
    void endNamespace(const Text &s = Text());

    void ifdef(const Text &s);
    void endif();

    void trimEnd(size_t n);

    Text getText() const;

    void setLines(const Lines &lines);
    Lines getLines() const;
    Lines &getLinesRef() { return lines; }
    void mergeBeforeAndAfterLines();

    void splitLines();
    void setMaxEmptyLines(int n);

    Context &before()
    {
        if (!before_)
            before_ = std::make_shared<Context>();
        return *before_;
    }
    Context &after()
    {
        if (!after_)
            after_ = std::make_shared<Context>();
        return *after_;
    }

    void emptyLines(int n);

    // add with "as is" indent
    Context &operator+=(const Context &rhs);
    // add with relative indent
    void addWithRelativeIndent(const Context &rhs);

    bool empty() const
    {
        bool e = false;
        if (before_)
            e |= before_->empty();
        e |= lines.empty();
        if (after_)
            e |= after_->empty();
        return e;
    }

    void printToFile(FILE* out) const;

    void define(const Text& str, const Text& def = Text())
    {
        addLine("#define " + str + " " + def + "\n");
    }

    void m4define(const Text& def, const Text& val = Text())
    {
        addLine("m4_define([[" + def + "]], [[" + val + "]])m4_dnl");
    }

    void m4undefine(const Text& def)
    {
        addLine("m4_undefine([[" + def + "]])m4_dnl");
    }

    template <typename T>
    Context& operator<<(const T &v)
    {
        ss_line << v;
        return *this;
    }
    Context& operator<<(const eol_type &)
    {
        addLine(ss_line.str());
        ss_line = decltype(ss_line)();
        return *this;
    }

private:
    Lines lines;
    std::shared_ptr<Context> before_;
    std::shared_ptr<Context> after_;

    int n_indents = 0;
    Text indent;
    Text newline;
    std::stack<Text> namespaces;
    std::ostringstream ss_line;
};

extern Context userdef_buf; /* a string buffer for #define's generated by user-options on cmd line. */
extern Context yydmap_buf;  /* a string buffer to hold yydmap elements */
extern Context m4defs_buf;  /* Holds m4 definitions. */
extern Context top_buf;     /* contains %top code. String buffer. */

extern Context processed_file;
