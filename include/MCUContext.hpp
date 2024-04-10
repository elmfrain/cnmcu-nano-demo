#ifndef MCU_CONTEXT_HPP
#define MCU_CONTEXT_HPP

#include "CodeNodeNano.hpp"

#include <fstream>
#include <string>
#include <thread>
#include <mutex>

#include <imgui.h>

// Quick hack to save window size, position and fullscreen state
// This should be moved to a separate file
struct WindowOptions
{
    ImVec2 size;
    ImVec2 pos;
    bool fullscreen;
};

class MCUContext
{
public:
    MCUContext();

    void start();
    void update(float dt);
    void terminate();

    static MCUContext& getInstance();

    CodeNodeNano mcu;

    uint32_t northInput, southInput, eastInput, westInput;
    uint32_t northOutput, southOutput, eastOutput, westOutput;
    double time;
    double lastTickTime;

    uint8_t northPower();
    uint8_t southPower();
    uint8_t eastPower();
    uint8_t westPower();

    void setCompileCommand(const char* command);
    bool compile(const std::string& code, const std::string& filename);
    bool upload(const std::string& code, const std::string& filename);
    bool doneCompiling();
    const std::string& getIDEstdout();
    void clearIDEstdout();

    static std::string loadCode(const char* filename);
    static void saveCode(const char* filename, const std::string& code);

    static std::string loadCompileCommand(const char* filename);
    static void saveCompileCommand(const char* filename, const std::string& command);

    static WindowOptions loadWindowOptions(const char* filename);
    static void saveWindowOptions(const char* filename, WindowOptions& options);
private:
    bool compiling;
    bool compileSuccess;
    bool shouldUpload;
    std::string binaryFilename;
    std::thread compileThread;
    std::mutex compileMutex;
    std::string ideStdout;
    const char* compileCommand = nullptr;

    void uploadToMCU();

    static void compileWorker(MCUContext* context, const std::string& code, const std::string& filename);
};

#endif