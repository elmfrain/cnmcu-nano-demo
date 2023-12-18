#include <Visualizer.hpp>

using namespace em;

static VisualizerApp* instance = nullptr;

VisualizerApp::VisualizerApp() :
    m_logger("VisualizerApp"),
    m_shouldClose(false),
    m_initialized(false)
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
    glfwWindowHint(GLFW_SAMPLES, options.msaaSamples);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, options.resizable);

    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(options.width, options.height, "Visualizer", nullptr, nullptr);
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

    // Initialize GLAD for OpenGL function loading
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        m_logger.fatalf("Unable to load OpenGL functions");
        return false;
    }

    // Setup input
    m_input = std::make_unique<Input>(m_window);

    // Setup callbacks
    glfwSetWindowSizeCallback(m_window, onWindowResize);

    // Setup scene
    m_scene.init();

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

    m_scene.draw();

    glfwSwapBuffers(m_window);
    glfwPollEvents();
    m_shouldClose = glfwWindowShouldClose(m_window);

    if(m_input->isKeyPressed(GLFW_KEY_ESCAPE))
        m_shouldClose = true;

    if(m_input->isKeyPressed(GLFW_KEY_F11))
        setFullscreen(!m_fullscreen);

    if(m_input->isKeyPressed(GLFW_KEY_F5))
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

    m_scene.destroy();

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

        glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        m_fullscreen = true;
    }
    else
    {
        glfwSetWindowMonitor(m_window, nullptr, 0, 0, m_params.width, m_params.height, GLFW_DONT_CARE);
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

void VisualizerApp::onWindowResize(GLFWwindow* window, int width, int height)
{
    VisualizerApp& app = VisualizerApp::getInstance();
    app.m_windowSize = glm::ivec2(width, height);
    app.m_scene.onWindowResize(width, height);
}