#include "modules/Parser.h"
#include "Parser.h"
#include <cstring>

namespace Modules
{
    bool Parser::Tray = false;
    bool Parser::Output = false;
    bool Parser::Input = false;
    void Parser::Parse(int argc, char **argv)
    {
        Tray = false;
        Output = false;
        Input = false;

        for (int i = 1; i < argc; ++i)
        {
            const char *arg = argv[i];
            if (std::strcmp(arg, "-t") == 0 || std::strcmp(arg, "--tray") == 0)
                Tray = true;
            else if (std::strcmp(arg, "-o") == 0 || std::strcmp(arg, "--output") == 0)
                Output = true;
            else if (std::strcmp(arg, "-i") == 0 || std::strcmp(arg, "--input") == 0)
                Input = true;
        }
    }
} // namespace Modules
