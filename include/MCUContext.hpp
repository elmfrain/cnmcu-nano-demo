#ifndef MCU_CONTEXT_HPP
#define MCU_CONTEXT_HPP

#include "CodeNodeNano.hpp"

#include <fstream>
#include <string>
#include <thread>
#include <mutex>

#include <stdio.h>
#include <sys/wait.h>
#include <glm/glm.hpp>

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

#ifdef MCU_CONTEXT_IMPL

MCUContext& MCUContext::getInstance()
{
    static MCUContext* instance = nullptr;

    if(instance == nullptr)
        instance = new MCUContext();

    return *instance;
}

MCUContext::MCUContext()
{
    mcu = CodeNodeNano();
    northInput = southInput = eastInput = westInput = 0;
    northOutput = southOutput = eastOutput = westOutput = 0;
    time = lastTickTime = 0.0;
    compiling = shouldUpload = false;
    compileSuccess = false;
}

void MCUContext::start()
{
}

void MCUContext::update(float dt)
{
    time += dt;

    if(time - lastTickTime >= 1.0 / GAME_TICK_RATE)
    {
        if(shouldUpload)
        {
            uploadToMCU();
            shouldUpload = false;
            return;
        }

        uint8_t* pvFront = mcu.GPIO().pvFrontData();
        uint8_t* pvBack = mcu.GPIO().pvBackData();
        uint8_t dir = *mcu.GPIO().dirData();
        bool northPinIsInput = (dir & 0b0001) == 0;
        bool eastPinIsInput =  (dir & 0b0010) == 0;
        bool southPinIsInput = (dir & 0b0100) == 0;
        bool westPinIsInput =  (dir & 0b1000) == 0;

        northOutput = northPinIsInput ? 0 : pvFront[0];
        eastOutput =  eastPinIsInput  ? 0 : pvFront[1];
        southOutput = southPinIsInput ? 0 : pvFront[2];
        westOutput =  westPinIsInput  ? 0 : pvFront[3];

        pvFront[0] = northPinIsInput ? northPower() : pvFront[0];
        pvFront[1] = eastPinIsInput  ? eastPower()  : pvFront[1];
        pvFront[2] = southPinIsInput ? southPower() : pvFront[2];
        pvFront[3] = westPinIsInput  ? westPower()  : pvFront[3];

        mcu.tick();
        lastTickTime = time;
    }
}

void MCUContext::terminate()
{
    mcu.powerOff();

    if(compileThread.joinable())
        compileThread.join();
}

uint8_t MCUContext::northPower()
{
    return glm::max(northInput, northOutput & 0xF) & 0xF;
}

uint8_t MCUContext::southPower()
{
    return glm::max(southInput, southOutput & 0xF) & 0xF;
}

uint8_t MCUContext::eastPower()
{
    return glm::max(eastInput, eastOutput & 0xF) & 0xF;
}

uint8_t MCUContext::westPower()
{
    return glm::max(westInput, westOutput & 0xF) & 0xF;
}

void MCUContext::setCompileCommand(const char* command)
{
    compileCommand = command;
}

bool MCUContext::compile(const std::string& code, const std::string& filename)
{
    if(compiling)
        return false;

    if(compileThread.joinable())
        compileThread.join();

    compiling = true;
    compileSuccess = false;
    compileThread = std::thread(compileWorker, this, code, filename);

    return true;
}

bool MCUContext::upload(const std::string& code, const std::string& filename)
{
    if(compile(code, filename))
    {
        shouldUpload = true;
        binaryFilename = filename;
        return true;
    }

    return false;
}

bool MCUContext::doneCompiling()
{
    return !compiling;
}

const std::string& MCUContext::getIDEstdout()
{
    return ideStdout;
}

void MCUContext::clearIDEstdout()
{
    std::unique_lock<std::mutex> lock(compileMutex);

    ideStdout = "";
}

std::string MCUContext::loadCode(const char* filename)
{
    std::ifstream file(filename);
    if(file.good())
    {
        std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return code;
    }
    
    file.close();

    return "";
}

void MCUContext::saveCode(const char* filename, const std::string& code)
{
    std::ofstream file(filename);

    if(file.good())
        file << code;

    file.close();
}

std::string MCUContext::loadCompileCommand(const char* filename)
{
    std::ifstream file(filename);

    if(file.good())
    {
        std::string command((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        return command;
    }
    
    file.close();

    return "";
}

void MCUContext::saveCompileCommand(const char* filename, const std::string& command)
{
    std::ofstream file(filename);

    if(file.good())
        file << command;

    file.close();
}

WindowOptions MCUContext::loadWindowOptions(const char* filename)
{
    std::ifstream file(filename);

    if(file.good())
    {
        WindowOptions options;

        file >> options.size.x >> options.size.y >> options.pos.x >> options.pos.y >> options.fullscreen;

        file.close();

        return options;
    }

    file.close();

    return { ImVec2(1280, 720), ImVec2(100, 100), false };
}

void MCUContext::saveWindowOptions(const char* filename, WindowOptions& options)
{
    std::ofstream file(filename);

    if(file.good())
        file << 
        options.size.x << " " << options.size.y << " " <<
        options.pos.x << " " << options.pos.y << " " << options.fullscreen;

    file.close();
}

void MCUContext::uploadToMCU()
{
    char infoBuffer[256] = {0};

    if(!compileSuccess)
    {
        snprintf(infoBuffer, 256, "\n[Uploader][Error]: Compilation failed\n");
        ideStdout += infoBuffer;
        return;
    }

    mcu.powerOff();

    std::ifstream file(binaryFilename, std::ios::binary | std::ios::ate);

    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    if(file.good())
    {
        if(size > mcu.ROM().size())
        {
            snprintf(infoBuffer, 256, "\n[Uploader][Error]: Binary too large, %lu bytes\n", size);
            ideStdout += infoBuffer;
            file.close();
            return;
        }
        else if(size < mcu.ROM().size())
        {
            snprintf(infoBuffer, 256, "\n[Uploader][Error]: Binary too small, %lu bytes\n", size);
            ideStdout += infoBuffer;
            file.close();
            return;
        }

        file.read(reinterpret_cast<char*>(mcu.ROM().data()), size);
    }
    else
    {
        snprintf(infoBuffer, 256, "\n[Uploader][Error]: Failed to open file \"%s\"\n", binaryFilename.c_str());
        ideStdout += infoBuffer;
        file.close();
        return;
    }

    snprintf(infoBuffer, 256, "\n[Uploader][Info]: Uploaded %lu bytes\n", size);
    ideStdout += infoBuffer;

    file.close();

    mcu.powerOn();

    compileSuccess = false;
}

void MCUContext::compileWorker(MCUContext* context, const std::string& code, const std::string& filename)
{
    char infoBuffer[256] = {0};

    if(context->compileCommand == nullptr)
    {
        context->ideStdout = "No compile command set";
        context->compiling = false;
        return;
    }

    std::string command = context->compileCommand;
    command += filename + " 2>&1";

    FILE* pipe = popen(command.c_str(), "r");
    if(!pipe)
    {
        context->ideStdout = "Failed to open pipe";
        context->compiling = false;
        return;
    }

    char buffer[128];
    context->ideStdout = "";
    while(!feof(pipe))
    {
        if(fgets(buffer, 128, pipe) != NULL)
        {
            std::unique_lock<std::mutex> lock(context->compileMutex);
            context->ideStdout += buffer;
        }
    }

    int status = pclose(pipe);
    if(WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        snprintf(infoBuffer, 256, "\n[Compiler][Info]: Compilation successful\n");
        context->ideStdout += infoBuffer;

        context->compileSuccess = true;
    }
    else
    {
        snprintf(infoBuffer, 256, "\n[Compiler][Error]: Compilation failed\n");
        context->ideStdout += infoBuffer;
    }

    context->compiling = false;
}

#endif