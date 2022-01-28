#include "win_debugcon.h"
#include "../../fmt/include/fmt/format.h"


class RDebugConsole : public DebugConsole
{
public:
    using DebugConsole::DebugConsole;
    
    template <typename... T> void Print(fmt::format_string<T...> fmt, T&&... args)
    {
        try {
            this->PrintInternal(fmt::format(fmt, std::forward<T>(args)...).c_str());
        } catch (const std::exception& e) {
            fprintf(stderr, "Error in the format: %s\n", e.what());
        }
    }

};


extern RDebugConsole *dbgCon;
