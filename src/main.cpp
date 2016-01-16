#include <locale>

#include "gettext.h"

int flex_main(int argc, char *argv[]);

/* Wrapper around flex_main, so flex_main can be built as a library. */
int main(int argc, char *argv[])
{
#if ENABLE_NLS
    setlocale(LC_MESSAGES, "");
    setlocale(LC_CTYPE, "");
    textdomain(PACKAGE);
    bindtextdomain(PACKAGE, LOCALEDIR);
#endif

    return flex_main(argc, argv);
}
