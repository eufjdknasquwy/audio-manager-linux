using System;

namespace Modules {
    public static class ArgsParser
    {
        public static bool Tray { get; private set; }
        public static bool Output { get; private set; }
        public static bool Input { get; private set; }

        static ArgsParser()
        {
            string[] args = Environment.GetCommandLineArgs();
            foreach (string arg in args)
            {
                if (arg == "-t" || arg == "--tray") Tray = true;
                if (arg == "-o" || arg == "--output") Output = true;
                if (arg == "-i" || arg == "--input") Input = true;
            }
        }
    }
}