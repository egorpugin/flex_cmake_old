// new file

#pragma once

#include <list>
#include <memory>
#include <string>
#include <stack>
#include <vector>

class Context
{
public:
    using Text = std::string;

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

private:
    Lines lines;
    std::shared_ptr<Context> before_;
    std::shared_ptr<Context> after_;

    int n_indents = 0;
    Text indent;
    Text newline;
    std::stack<Text> namespaces;
};
