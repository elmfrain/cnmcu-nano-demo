#pragma once

#include <string>

namespace em
{
    class Logger
    {
    public:
        Logger();
        Logger(const char* moduleName);

        const std::string& getModuleName() const;

        Logger& submodule(const char* submodule);

        Logger& infof(const char* fmt, ...);
        Logger& warnf(const char* fmt, ...);
        Logger& errorf(const char* fmt, ...);
        Logger& fatalf(const char* fmt, ...);
    private:
        std::string m_moduleName;
        std::string m_submoduleName;
    };
}