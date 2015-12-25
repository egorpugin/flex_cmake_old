// new file

#include "context.h"

Context::Lines &operator+=(Context::Lines &s1, const Context::Lines &s2)
{
    s1.insert(s1.end(), s2.begin(), s2.end());
    return s1;
}

Context::Lines operator+(const Context::Lines &s1, const Context::Lines &s2)
{
    Context::Lines s(s1.begin(), s1.end());
    return s += s2;
}

Context::Context(const Text &indent, const Text &newline)
    : indent(indent), newline(newline)
{
}

void Context::addText(const Text &s)
{
    if (lines.empty())
        lines.emplace_back();
    lines.back() += s;
}

void Context::addNoNewLine(const Text &s)
{
    lines.push_back(Line{ s, n_indents });
}

void Context::addLineNoSpace(const Text & s)
{
    lines.push_back(Line{ s });
}

void Context::addLine(const Text &s)
{
    if (s.empty())
        lines.push_back(Line{});
    else
        lines.push_back({ s, n_indents });
}

void Context::decreaseIndent()
{
    n_indents--;
}

void Context::increaseIndent()
{
    n_indents++;
}

void Context::beginBlock(const Text &s, bool indent)
{
    if (!s.empty())
        addLine(s);
    addLine("{");
    if (indent)
        increaseIndent();
}

void Context::endBlock(bool semicolon)
{
    decreaseIndent();
    addLine(semicolon ? "};" : "}");
}

void Context::beginFunction(const Text &s)
{
    beginBlock(s);
}

void Context::endFunction()
{
    endBlock();
    addLine();
}

void Context::beginNamespace(const Text &s)
{
    addLineNoSpace("namespace " + s);
    addLineNoSpace("{");
    addLine();
    namespaces.push(s);
}

void Context::endNamespace(const Text &ns)
{
    Text s = ns;
    if (!namespaces.empty() && ns.empty())
    {
        s = namespaces.top();
        namespaces.pop();
    }
    addLineNoSpace("} // namespace " + s);
    addLine();
}

void Context::ifdef(const Text &s)
{
    addLineNoSpace("#ifdef " + s);
}

void Context::endif()
{
    addLineNoSpace("#endif");
}

void Context::trimEnd(size_t n)
{
    if (lines.empty())
        return;
    auto &t = lines.back().text;
    auto sz = t.size();
    if (n > sz)
        n = sz;
    t.resize(sz - n);
}

Context::Text Context::getText() const
{
    Text s;
    auto lines = getLines();
    for (auto &line : lines)
    {
        Text space;
        for (int i = 0; i < line.n_indents; i++)
            space += indent;
        s += space + line.text + newline;
    }
    return s;
}

Context::Lines Context::getLines() const
{
    Lines lines;
    if (before_)
        lines += before_->getLines();
    lines += this->lines;
    if (after_)
        lines += after_->getLines();
    return lines;
}

void Context::emptyLines(int n)
{
    int e = 0;
    for (auto i = lines.rbegin(); i != lines.rend(); ++i)
    {
        if (i->text.empty())
            e++;
        else
            break;
    }
    if (e < n)
    {
        while (e++ != n)
            addLine();
    }
    else if (e > n)
    {
        lines.resize(lines.size() - (e - n));
    }
}

Context &Context::operator+=(const Context &rhs)
{
    if (before_ && rhs.before_)
        before_->lines += rhs.before_->lines;
    else if (rhs.before_)
    {
        before().lines += rhs.before_->lines;
    }
    lines += rhs.lines;
    if (after_ && rhs.after_)
        after_->lines += rhs.after_->lines;
    else if (rhs.after_)
    {
        after().lines += rhs.after_->lines;
    }
    return *this;
}

void Context::addWithRelativeIndent(const Context &rhs)
{
    auto addWithRelativeIndent = [this](Lines &l1, Lines l2)
    {
        for (auto &l : l2)
            l.n_indents += n_indents;
        l1 += l2;
    };

    if (before_ && rhs.before_)
        addWithRelativeIndent(before_->lines, rhs.before_->lines);
    else if (rhs.before_)
    {
        addWithRelativeIndent(before().lines, rhs.before_->lines);
    }
    addWithRelativeIndent(lines, rhs.lines);
    if (after_ && rhs.after_)
        addWithRelativeIndent(after_->lines, rhs.after_->lines);
    else if (rhs.after_)
    {
        addWithRelativeIndent(after().lines, rhs.after_->lines);
    }
}
