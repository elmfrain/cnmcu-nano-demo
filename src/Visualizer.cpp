#include <Visualizer.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>
#include <imgui_memory_editor.h>

#include "MCUContext.hpp"

using namespace em;

static VisualizerApp* instance = nullptr;
static std::string compileCommand = MCUContext::loadCompileCommand("res/compile_command.ini");

VisualizerApp::VisualizerApp() :
    m_logger("CNMCU Nano Demo App"),
    m_shouldClose(false),
    m_initialized(false),
    m_mcuContext(MCUContext::getInstance())
{
    if(instance)
        m_logger.warnf("Creating another instance when a VisualizerApp instance already exists");

    instance = this;
}

bool VisualizerApp::start(AppParams& options)
{
    m_params = options;
    if(!glfwInit())
    {
        m_logger.fatalf("Unable to initialize GLFW");
        return false;
    }

    // Setup Glfw window
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    m_window = glfwCreateWindow(options.width, options.height, "CodeNode Nano Demo", nullptr, nullptr);
    m_windowSize = glm::ivec2(options.width, options.height);

    if(!m_window)
    {
        m_logger.fatalf("Unable to create GLFW window");
        return false;
    }

    glfwSetWindowSizeLimits(m_window, options.minWidth, options.minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    setFullscreen(options.fullscreen);
    glfwSwapInterval(options.vsync);

    glfwMakeContextCurrent(m_window);

#ifndef EMSCRIPTEN
    // Initialize GLAD for OpenGL function loading
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        m_logger.fatalf("Unable to load OpenGL functions");
        return false;
    }
#endif

    m_logger.infof("OpenGL Version: %s", glGetString(GL_VERSION));

    // Setup input
    m_input = std::make_unique<Input>(m_window);

    // Setup callbacks
    glfwSetWindowSizeCallback(m_window, onWindowResize);

    // Setup scene
    m_scene.init();
    m_mcuContext.start();
    m_mcuContext.setCompileCommand(compileCommand.c_str());

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    m_textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
    m_textEditor.SetText(MCUContext::loadCode("res/program.s"));

    // Load window options
    WindowOptions windowOptions = MCUContext::loadWindowOptions("res/window-options.ini");

    if(windowOptions.fullscreen)
        setFullscreen(true);

    glfwSetWindowSize(m_window, windowOptions.size.x, windowOptions.size.y);
    glfwSetWindowPos(m_window, windowOptions.pos.x, windowOptions.pos.y);

    m_initialized = true;

    return true;
}

bool VisualizerApp::shouldClose()
{
    return m_shouldClose;
}

bool VisualizerApp::runLoop()
{
    if(!m_initialized)
    {
        m_logger.errorf("Application has not been initialized");
        return false;
    }

    double currentTime = glfwGetTime();
    float dt = currentTime - m_lastFrameTime;
    m_lastFrameTime = currentTime;
    m_scene.update(dt);
    m_input->update();
    glfwPollEvents();
    m_mcuContext.update(dt);

    m_scene.draw();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    genUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
    
    m_shouldClose = glfwWindowShouldClose(m_window);

    if(ImGui::GetIO().WantCaptureKeyboard)
        return true;

    if(m_input->isKeyPressed(GLFW_KEY_ESCAPE))
        m_shouldClose = true;

    if(m_input->isKeyPressed(GLFW_KEY_F11))
        setFullscreen(!m_fullscreen);

    if(m_input->isKeyPressed(GLFW_KEY_R))
        m_scene.reload();

    return true;
}

bool VisualizerApp::terminate()
{
    if(!m_initialized)
    {
        m_logger.errorf("Application has not been initialized");
        return false;
    }

    // Save window options
    WindowOptions options;
    int width, height, x, y;
    glfwGetWindowSize(m_window, &width, &height);
    glfwGetWindowPos(m_window, &x, &y);
    options.fullscreen = m_fullscreen;
    options.size = ImVec2(width, height);
    options.pos = ImVec2(x, y);
    MCUContext::saveWindowOptions("res/window-options.ini", options);

    m_scene.destroy();
    m_mcuContext.terminate();

    glfwTerminate();

    return true;
}

void VisualizerApp::setFullscreen(bool fullscreen)
{
    if(m_fullscreen == fullscreen)
        return;

    if(fullscreen)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // Save window options
        WindowOptions options;
        int width, height, x, y;
        glfwGetWindowSize(m_window, &width, &height);
        glfwGetWindowPos(m_window, &x, &y);
        options.fullscreen = m_fullscreen;
        options.size = ImVec2(width, height);
        options.pos = ImVec2(x, y);
        MCUContext::saveWindowOptions("res/window-options.ini", options);

        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        m_fullscreen = true;
    }
    else
    {
        WindowOptions options = MCUContext::loadWindowOptions("res/window-options.ini");

        glfwSetWindowMonitor(m_window, nullptr, 0, 0, options.size.x, options.size.y, GLFW_DONT_CARE);
        glfwSetWindowPos(m_window, options.pos.x, options.pos.y);
        
        m_fullscreen = false;
    }
}

glm::ivec2 VisualizerApp::getWindowSize()
{
    return m_windowSize;
}

AppParams VisualizerApp::getParams()
{
    return m_params;
}

const Input& VisualizerApp::getInput()
{
    return *m_input;
}

GLFWwindow* VisualizerApp::getWindowHandle()
{
    return m_window;
}

VisualizerApp& VisualizerApp::getInstance()
{
    return *instance;
}

void VisualizerApp::genUI()
{
    static bool showAbout = false;
    static bool showDocumentation = false;
    static bool showKeybinds = false;

    ImGui::SetNextWindowSize(ImVec2(350, 422), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_FirstUseEver);
    ImGui::Begin("CodeNode Nano Demo", nullptr, ImGuiWindowFlags_MenuBar);

    ImGui::BeginMenuBar();
    if(ImGui::BeginMenu("Help"))
    {
        if(ImGui::MenuItem("About"))
            showAbout = true;
        if(ImGui::MenuItem("Documentation"))
            showDocumentation = true;
        if(ImGui::MenuItem("Keybinds"))
            showKeybinds = true;
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();

    if(ImGui::BeginPopupModal("About"))
        {
            m_logger.infof("hereo");
            ImGui::Text("CNMCU Nano Demo");
            ImGui::Text("Version 1.0.0");
            ImGui::Text("Author: CodeNode");
            ImGui::Text("License: MIT");
            if(ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Window Size: %d x %d", m_windowSize.x, m_windowSize.y);

    ImGui::SeparatorText("Specs");
    ImGui::Text("CPU: mos6502");
    ImGui::Text("RAM: %.1fKB", m_mcuContext.mcu.RAM_SIZE / 1024.0);
    ImGui::Text("ROM: %.1fKB", m_mcuContext.mcu.ROM_SIZE / 1024.0);
    ImGui::Text("Clock Speed: %dHz", (int) m_mcuContext.mcu.CLOCK_FREQUENCY);
    ImGui::Text("Modules: GPIO");

    ImGui::SeparatorText("Controls");
    static bool isMCUPowered = m_mcuContext.mcu.isPoweredOn();
    isMCUPowered = m_mcuContext.mcu.isPoweredOn();
    if(ImGui::Checkbox("Power", &isMCUPowered))
    {
        if(isMCUPowered)
            m_mcuContext.mcu.powerOn();
        else
            m_mcuContext.mcu.powerOff();
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset"))
        m_mcuContext.mcu.reset();
    ImGui::SameLine();
    static bool isClockPaused = m_mcuContext.mcu.isClockPaused();
    isClockPaused = m_mcuContext.mcu.isClockPaused();
    if(ImGui::Checkbox("Pause Clock", &isClockPaused))
    {
        if(isClockPaused)
            m_mcuContext.mcu.pauseClock();
        else
            m_mcuContext.mcu.resumeClock();
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(!m_mcuContext.mcu.isClockPaused());
    if(ImGui::Button("Cycle"))
        m_mcuContext.mcu.cycle();
    ImGui::EndDisabled();
    ImGui::SliderInt("North Input", (int*) &m_mcuContext.northInput, 0, 15);
    ImGui::SliderInt("South Input", (int*) &m_mcuContext.southInput, 0, 15);
    ImGui::SliderInt("East Input", (int*) &m_mcuContext.eastInput, 0, 15);
    ImGui::SliderInt("West Input", (int*) &m_mcuContext.westInput, 0, 15);

    ImGui::SeparatorText("Pins");
    ImGui::Text("North: %d", m_mcuContext.northPower());
    ImGui::Text("South: %d", m_mcuContext.southPower());
    ImGui::Text("East: %d", m_mcuContext.eastPower());
    ImGui::Text("West: %d", m_mcuContext.westPower());

    ImGui::End();

    genTextEditor();
    genCPUStatus();
    genZeroPageView();

    if(showAbout)
    {
        ImGui::OpenPopup("About");
        showAbout = false;
    }
    if(showDocumentation)
    {
        ImGui::OpenPopup("Documentation");
        showDocumentation = false;
    }
    if(showKeybinds)
    {
        ImGui::OpenPopup("Keybinds");
        showKeybinds = false;
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if(ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("CodeNode Microcontrollers - Nano Demo v%s", CNMCU_DEMO_VERSION);        
        ImGui::Text("Author: elmfer");
        ImGui::Text("License: GNU GPL v3");
        ImGui::Text("GitHub Repo: elmfrain/cnmcu-nano-demo");
        ImGui::Separator();
        ImGui::Text("CNMCU Nano Demo is a proof-of-concept of the CodeNode Microcontrollers project.");
        ImGui::Text("The CNMCU project is a microcontroller emulator for use in Minecraft as a mod.");
        if(ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(800, 500), ImGuiCond_Once);

    if(ImGui::BeginPopupModal("Documentation"))
    {
        genDocumenation();
        if(ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if(ImGui::BeginPopupModal("Keybinds", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Keybinds");
        ImGui::Separator();
        
        if(ImGui::BeginTable("Keybinds", 2, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Key");
            ImGui::TableSetupColumn("Action");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("F11");
            ImGui::TableNextColumn();
            ImGui::Text("Toggle Fullscreen");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("ESC");
            ImGui::TableNextColumn();
            ImGui::Text("Close Application");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("R");
            ImGui::TableNextColumn();
            ImGui::Text("Reload Scene");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Tab");
            ImGui::TableNextColumn();
            ImGui::Text("Lock/Unlock Mouse");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("W");
            ImGui::TableNextColumn();
            ImGui::Text("Move Forward");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("A");
            ImGui::TableNextColumn();
            ImGui::Text("Move Left");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("S");
            ImGui::TableNextColumn();
            ImGui::Text("Move Backward");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("D");
            ImGui::TableNextColumn();
            ImGui::Text("Move Right");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Space");
            ImGui::TableNextColumn();
            ImGui::Text("Move Up");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("LShift");
            ImGui::TableNextColumn();
            ImGui::Text("Move Down");

            ImGui::EndTable();
        }

        if(ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void VisualizerApp::genTextEditor()
{   
    ImGuiIO& io = ImGui::GetIO();
    static bool isSaved = true;

    ImGui::SetNextWindowPos(ImVec2(774, 12), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(500, 700), ImGuiCond_FirstUseEver);

    if(!ImGui::Begin("CodeNode IDE", nullptr, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Save"))
            {
                MCUContext::saveCode("res/program.s", m_textEditor.GetText());
                isSaved = true;
            }
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit"))
        {
            if(ImGui::MenuItem("Undo", "CTRL+Z"))
                m_textEditor.Undo();
            if(ImGui::MenuItem("Redo", "CTRL+Y"))
                m_textEditor.Redo();
            ImGui::Separator();
            if(ImGui::MenuItem("Copy", "CTRL+C"))
                m_textEditor.Copy();
            if(ImGui::MenuItem("Cut", "CTRL+X"))
                m_textEditor.Cut();
            if(ImGui::MenuItem("Paste", "CTRL+V"))
                m_textEditor.Paste();
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Select"))
        {
            if(ImGui::MenuItem("Select All", "CTRL+A"))
                m_textEditor.SetSelection(TextEditor::Coordinates(), {m_textEditor.GetTotalLines(), 0});
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Tools"))
        {
            if(ImGui::MenuItem("Compile"))
            {
                MCUContext::saveCode("res/program.s", m_textEditor.GetText());
                isSaved = true;
                m_mcuContext.compile(m_textEditor.GetText(), "res/rom.bin");
            }
            if(ImGui::MenuItem("Upload"))
            {
                MCUContext::saveCode("res/program.s", m_textEditor.GetText());
                isSaved = true;
                m_mcuContext.upload(m_textEditor.GetText(), "res/rom.bin");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if(io.KeysDown[GLFW_KEY_LEFT_CONTROL] && io.KeysDown[GLFW_KEY_Z])
        m_textEditor.Undo();
    if(io.KeysDown[GLFW_KEY_LEFT_CONTROL] && io.KeysDown[GLFW_KEY_Y])
        m_textEditor.Redo();
    if(io.KeysDown[GLFW_KEY_LEFT_CONTROL] && io.KeysDown[GLFW_KEY_C])
        m_textEditor.Copy();
    if(io.KeysDown[GLFW_KEY_LEFT_CONTROL] && io.KeysDown[GLFW_KEY_X])
        m_textEditor.Cut();
    if(io.KeysDown[GLFW_KEY_LEFT_CONTROL] && io.KeysDown[GLFW_KEY_V])
        m_textEditor.Paste();
    if(io.KeysDown[GLFW_KEY_LEFT_CONTROL] && io.KeysDown[GLFW_KEY_A])
        m_textEditor.SetSelection(TextEditor::Coordinates(), {m_textEditor.GetTotalLines(), 0});

    ImGui::BeginDisabled(!m_mcuContext.doneCompiling());
    if(ImGui::Button("Compile"))
    {
        MCUContext::saveCode("res/program.s", m_textEditor.GetText());
        isSaved = true;
        m_mcuContext.compile(m_textEditor.GetText(), "res/rom.bin");

    }
    ImGui::SameLine();
    if(ImGui::Button("Upload"))
    {
        MCUContext::saveCode("res/program.s", m_textEditor.GetText());
        isSaved = true;
        m_mcuContext.upload(m_textEditor.GetText(), "res/rom.bin");
    }
    ImGui::SameLine();
    if(ImGui::InputText("##", &compileCommand))
    {
        MCUContext::saveCompileCommand("res/compile_command.ini", compileCommand);
        m_mcuContext.setCompileCommand(compileCommand.c_str());
    }
    ImGui::EndDisabled();

    ImGui::Text("res/program.s %s", isSaved ? "[Saved]" : "[Unsaved]");
    m_textEditor.Render("CodeNodeNano ROM", ImVec2(0, 400));
    if(m_textEditor.IsTextChanged())
        isSaved = false;
    ImGui::BeginChild("Output", ImVec2(0, 200), ImGuiChildFlags_Border);
    ImGui::Text("Output");
    ImGui::SameLine();
    if(ImGui::Button("Clear"))
        m_mcuContext.clearIDEstdout();
    ImGui::Separator();
    ImGui::Text("%s", m_mcuContext.getIDEstdout().c_str());
    ImGui::EndChild();

    ImGui::End();
}

void VisualizerApp::genCPUStatus()
{
    static bool showHex = true;

    ImGui::SetNextWindowPos(ImVec2(12, 439), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("CPU Status"))
    {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Registers in Hex", &showHex);

    if(showHex)
    {
        ImGui::Text("A: 0x%02X", m_mcuContext.mcu.CPU().GetA());
        ImGui::Text("X: 0x%02X", m_mcuContext.mcu.CPU().GetX());
        ImGui::Text("Y: 0x%02X", m_mcuContext.mcu.CPU().GetY());
    }
    else
    {
        ImGui::Text("A: %d", m_mcuContext.mcu.CPU().GetA());
        ImGui::Text("X: %d", m_mcuContext.mcu.CPU().GetX());
        ImGui::Text("Y: %d", m_mcuContext.mcu.CPU().GetY());
    }

    uint8_t status = m_mcuContext.mcu.CPU().GetP();
    ImGui::Text("PC: 0x%04X", m_mcuContext.mcu.CPU().GetPC());
    ImGui::Text("SP: 0x%02X", m_mcuContext.mcu.CPU().GetS());
    ImGui::Text("Flags: %c%c%c%c%c%c%c%c",
        status & 0b10000000 ? 'N' : '-',
        status & 0b01000000 ? 'V' : '-',
        status & 0b00100000 ? 'U' : '-',
        status & 0b00010000 ? 'B' : '-',
        status & 0b00001000 ? 'D' : '-',
        status & 0b00000100 ? 'I' : '-',
        status & 0b00000010 ? 'Z' : '-',
        status & 0b00000001 ? 'C' : '-'
    );
    ImGui::Text("Cycles: %lu", m_mcuContext.mcu.numCycles());
    ImGui::SeparatorText("Bus");
    ImGui::Text("Address: 0x%04X", m_mcuContext.mcu.busAddress());
    ImGui::Text("Data: 0x%02X", m_mcuContext.mcu.busData());
    ImGui::Text("RW: %s", m_mcuContext.mcu.busRw() ? "Write" : "Read");

    ImGui::End();
}

void VisualizerApp::genZeroPageView()
{
    ImGui::SetNextWindowPos(ImVec2(175, 439), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(388, 272), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Zero Page View"))
    {
        ImGui::End();
        return;
    }

    static MemoryEditor memEdit;
    memEdit.DrawContents(m_mcuContext.mcu.RAM().data(), 256);

    ImGui::End();
}

void VisualizerApp::genDocumenation()
{
    ImGui::SeparatorText("Memory Map");
    ImGui::Text("The CNMCU Nano has a 64KB address space, with 8KB of ROM and 512 bytes of RAM.");

    if(ImGui::BeginTable("Memory Map", 3, ImGuiTableFlags_Borders))
    {
        ImGui::TableSetupColumn("Addresses");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Uses");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "$0000 - $01FF");
        ImGui::TableNextColumn();
        ImGui::Text("RAM");
        ImGui::TableNextColumn();
        ImGui::Text("256 byte Zero Page | 256 byte Stack");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7000 - $7070");
        ImGui::TableNextColumn();
        ImGui::Text("GPIO");
        ImGui::TableNextColumn();
        ImGui::Text("Pin Control/Interrupts");

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "$E000 - $FFFF");
        ImGui::TableNextColumn();
        ImGui::Text("ROM");
        ImGui::TableNextColumn();
        ImGui::Text("Program Memory | Processor Vectors");

        ImGui::EndTable();
    }

    ImGui::NewLine();
    ImGui::SeparatorText("Registers");

    if(ImGui::CollapsingHeader("GPIOPV (Pin Value Registers)"))
    {
        ImGui::Text("There are 64 GPIOPV registers, each representing a pin. Only 4 are used.");
        ImGui::Text("Read: Returns the value of the pin");
        ImGui::Text("Write: Sets the value of the pin (if it is an output)");
        ImGui::SeparatorText("Register Table");

        if(ImGui::BeginTable("GPIOPV", 4, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Read/Write");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7000");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOPV0");
            ImGui::TableNextColumn();
            ImGui::Text("Front pin value");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7001");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOPV1");
            ImGui::TableNextColumn();
            ImGui::Text("Right pin value");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7002");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOPV2");
            ImGui::TableNextColumn();
            ImGui::Text("Back pin value");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7003");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOPV3");
            ImGui::TableNextColumn();
            ImGui::Text("Left pin value");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::EndTable();
        }
    }

    if(ImGui::CollapsingHeader("GPIODIR (Pin Direction Registers)"))
    {
        ImGui::Text("There are 8 GPIODIR registers, each bit representing a pin. Only 1 is used.");
        ImGui::Text("Read: Returns the value of the register");
        ImGui::Text("Write: Sets the value of the register");
        ImGui::SeparatorText("Register Table");

        if(ImGui::BeginTable("GPIODIR", 4, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Read/Write");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7040");
            ImGui::TableNextColumn();
            ImGui::Text("GPIODIR0");
            ImGui::TableNextColumn();
            ImGui::Text("Pins (0 - 7) direction");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::EndTable();
        }

        ImGui::NewLine();
        ImGui::SeparatorText("GPIODIR0 Register (N/U - Not Used)");

        if(ImGui::BeginTable("GPIODIR0", 8, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Bit 7");
            ImGui::TableSetupColumn("6");
            ImGui::TableSetupColumn("5");
            ImGui::TableSetupColumn("4");
            ImGui::TableSetupColumn("3");
            ImGui::TableSetupColumn("2");
            ImGui::TableSetupColumn("1");
            ImGui::TableSetupColumn("0");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("Left Dir");
            ImGui::TableNextColumn();
            ImGui::Text("Back Dir");
            ImGui::TableNextColumn();
            ImGui::Text("Right Dir");
            ImGui::TableNextColumn();
            ImGui::Text("Front Dir");

            ImGui::EndTable();
        }
    }

    if(ImGui::CollapsingHeader("GPIOINT (Pin Interrupt Type Registers)"))
    {
        ImGui::Text("There are 32 GPIOINT registers, each nibble representing a pin. Only 4 are used.");
        ImGui::Text("Read: Returns the value of the register");
        ImGui::Text("Write: Sets the value of the register");
        ImGui::SeparatorText("Register Table");

        if(ImGui::BeginTable("GPIOINT", 4, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Read/Write");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7048");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOINT0");
            ImGui::TableNextColumn();
            ImGui::Text("Front/Right interrupt type");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7049");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOINT1");
            ImGui::TableNextColumn();
            ImGui::Text("Back/Left interrupt type");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::EndTable();
        }

        ImGui::NewLine();
        ImGui::SeparatorText("Interrupt Types");

        if(ImGui::BeginTable("InterruptTypes", 3, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Value");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Description");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$0");
            ImGui::TableNextColumn();
            ImGui::Text("NO_INTERRUPT");
            ImGui::TableNextColumn();
            ImGui::Text("Disable interupts");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$1");
            ImGui::TableNextColumn();
            ImGui::Text("LOW");
            ImGui::TableNextColumn();
            ImGui::Text("Trigger while low");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$2");
            ImGui::TableNextColumn();
            ImGui::Text("HIGH");
            ImGui::TableNextColumn();
            ImGui::Text("Trigger while high");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$3");
            ImGui::TableNextColumn();
            ImGui::Text("RISING");
            ImGui::TableNextColumn();
            ImGui::Text("On rising edge");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$4");
            ImGui::TableNextColumn();
            ImGui::Text("FALLING");
            ImGui::TableNextColumn();
            ImGui::Text("On falling edge");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$5");
            ImGui::TableNextColumn();
            ImGui::Text("CHANGE");
            ImGui::TableNextColumn();
            ImGui::Text("Digital change");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$6");
            ImGui::TableNextColumn();
            ImGui::Text("ANALOG_CHANGE");
            ImGui::TableNextColumn();
            ImGui::Text("On pin value change");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7");
            ImGui::TableNextColumn();
            ImGui::Text("ANALOG_RISING");
            ImGui::TableNextColumn();
            ImGui::Text("On pin value increase");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$8");
            ImGui::TableNextColumn();
            ImGui::Text("ANALOG_FALLING");
            ImGui::TableNextColumn();
            ImGui::Text("On pin value decrease");

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$9");
            ImGui::TableNextColumn();
            ImGui::Text("NO_CHANGE");
            ImGui::TableNextColumn();
            ImGui::Text("Trigger while no change");

            ImGui::EndTable();
        }

        ImGui::NewLine();
        ImGui::SeparatorText("GPIOINT0 Register");

        if(ImGui::BeginTable("GPIOINT0", 2, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Bits 7 - 4");
            ImGui::TableSetupColumn("Bits 3 - 0");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Right Pin Interrupt Type");
            ImGui::TableNextColumn();
            ImGui::Text("Front Pin Interrupt Type");

            ImGui::EndTable();
        }

        ImGui::NewLine();
        ImGui::SeparatorText("GPIOINT1 Register");

        if(ImGui::BeginTable("GPIOINT1", 2, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Bits 7 - 4");
            ImGui::TableSetupColumn("Bits 3 - 0");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Left Pin Interrupt Type");
            ImGui::TableNextColumn();
            ImGui::Text("Back Pin Interrupt Type");

            ImGui::EndTable();
        }
    }

    if(ImGui::CollapsingHeader("GPIOIFL (Pin Interrupt Flag Registers)"))
    {
        ImGui::Text("There are 8 GPIOIFL registers, each bit representing a pin. Only 1 is used.");
        ImGui::Text("If any flag is set, the IRQ line is held low. The flag must be cleared after the interrupt is handled.");
        ImGui::Text("Read: Returns the value of the register");
        ImGui::Text("Write: Clear bits of the register");
        ImGui::SeparatorText("Register Table");

        if(ImGui::BeginTable("GPIOIFL", 4, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Address");
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Description");
            ImGui::TableSetupColumn("Read/Write");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "$7068");
            ImGui::TableNextColumn();
            ImGui::Text("GPIOIFL0");
            ImGui::TableNextColumn();
            ImGui::Text("Front/Right interrupt flag");
            ImGui::TableNextColumn();
            ImGui::Text("R/W");

            ImGui::EndTable();
        }

        ImGui::NewLine();
        ImGui::SeparatorText("GPIOIFL0 Register (N/U - Not Used)");

        if(ImGui::BeginTable("GPIOIFL0", 8, ImGuiTableFlags_Borders))
        {
            ImGui::TableSetupColumn("Bit 7");
            ImGui::TableSetupColumn("6");
            ImGui::TableSetupColumn("5");
            ImGui::TableSetupColumn("4");
            ImGui::TableSetupColumn("3");
            ImGui::TableSetupColumn("2");
            ImGui::TableSetupColumn("1");
            ImGui::TableSetupColumn("0");
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("N/U");
            ImGui::TableNextColumn();
            ImGui::Text("Left Flag");
            ImGui::TableNextColumn();
            ImGui::Text("Back Flag");
            ImGui::TableNextColumn();
            ImGui::Text("Right Flag");
            ImGui::TableNextColumn();
            ImGui::Text("Front Flag");

            ImGui::EndTable();
        }
    }

    ImGui::NewLine();
    ImGui::SeparatorText("More Info");
    ImGui::Text("For more information, please refer to the CodeNode Microcontrollers documentation.");
    ImGui::Text("https://elmfrain.github.io/code-node-docs/");
    ImGui::SameLine();
    if(ImGui::Button("Copy Link")) 
        ImGui::SetClipboardText("https://elmfrain.github.io/code-node-docs/");
}

void VisualizerApp::onWindowResize(GLFWwindow* window, int width, int height)
{
    VisualizerApp& app = VisualizerApp::getInstance();
    app.m_windowSize = glm::ivec2(width, height);
    app.m_scene.onWindowResize(width, height);
}