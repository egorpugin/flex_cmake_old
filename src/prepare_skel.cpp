#include <fstream>
#include <iostream>

#include "flexdef.h"
#include "simple_m4.h"
#include "version.h"

const simple_m4::M4Functions preprocess_functions =
{
    { "m4preproc_changecom", simple_m4::MacroType::changecom },
    { "m4preproc_define", simple_m4::MacroType::define },
    { "m4preproc_include", simple_m4::MacroType::include },
};

int main(int argc, char *argv[])
try
{
    if (argc != 3)
    {
        std::cerr << "Usage: prepare_skel flex.skl skel.cpp" << "\n";
        return 1;
    }

    auto s = simple_m4::read_file(argv[1]);
    Context ctx;
    ctx.addLine(s);
    ctx.splitLines();

    simple_m4::M4 processor;
    processor.setFunctions(preprocess_functions);

    int major, minor, subminor;
    sscanf(FLEX_VERSION, "%d.%d.%d", &major, &minor, &subminor);
    processor.addDefinition("FLEX_MAJOR_VERSION", std::to_string(major));
    processor.addDefinition("FLEX_MINOR_VERSION", std::to_string(minor));
    processor.addDefinition("FLEX_SUBMINOR_VERSION", std::to_string(subminor));

    for (auto &line : ctx.getLinesRef())
        line.text = processor.processLine(line.text);

    ctx.splitLines();
    //ctx.setMaxEmptyLines(1);

    for (auto &line : ctx.getLinesRef())
    {
        line.text = "R\"skel(" + line.text + ")skel\",";
        line.n_indents = 1;
    }

    ctx.before().addLine("/* File created from flex.skl via prepare_skel */");
    ctx.before().addLine();
    ctx.before().addLine("#include <flexdef.h>");
    ctx.before().addLine();
    ctx.before().addLine("const char *skel[] = {");

    ctx.after().increaseIndent();
    ctx.after().addLine("0");
    ctx.after().decreaseIndent();
    ctx.after().addLine("};");

    std::ofstream ofile(argv[2]);
    if (ofile)
        ofile << ctx.getText();

    return 0;
}
catch (const std::exception &e)
{
    std::cerr << e.what() << "\n";
    return 1;
}
