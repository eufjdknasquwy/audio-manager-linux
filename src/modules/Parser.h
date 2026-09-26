#pragma once

namespace Modules
{
    class Parser
    {
        public:
            static void Parse(int argc, char *argv[]);

            static bool Tray;
            static bool Output;
            static bool Input;

            Parser() = delete;
    };
} // namespace Modules
