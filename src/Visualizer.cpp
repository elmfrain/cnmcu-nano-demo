#include <Visualizer.hpp>

using namespace em;

VisualizerApp::VisualizerApp() :
    m_logger("VisualizerApp"),
    m_shouldClose(false),
    m_initialized(false)
{

}

bool VisualizerApp::start(AppParams& options)
{
    m_params = options;

    if(!glfwInit())
    {
        m_logger.fatalf("Unable to initialize GLFW");
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, options.msaaSamples);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, options.resizable);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(options.width, options.height, "Visualizer", nullptr, nullptr);

    if(!m_window)
    {
        m_logger.fatalf("Unable to create GLFW window");
        return false;
    }

    glfwSetWindowSizeLimits(m_window, options.minWidth, options.minHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    setFullscreen(options.fullscreen);
    glfwSwapInterval(options.vsync);

    glfwMakeContextCurrent(m_window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        m_logger.fatalf("Unable to load OpenGL functions");
        return false;
    }

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

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwSwapBuffers(m_window);
    glfwPollEvents();
    m_shouldClose = glfwWindowShouldClose(m_window);

    if(glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
        m_shouldClose = true;

    if(glfwGetKey(m_window, GLFW_KEY_F11) == GLFW_RELEASE)
        setFullscreen(!m_fullscreen);

    return true;
}

bool VisualizerApp::terminate()
{
    if(!m_initialized)
    {
        m_logger.errorf("Application has not been initialized");
        return false;
    }

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
    if(!m_initialized)
    {
        m_logger.errorf("Application has not been initialized");
        return glm::ivec2(0, 0);
    }

    int width, height;
    glfwGetWindowSize(m_window, &width, &height);

    return glm::ivec2(width, height);
}

AppParams VisualizerApp::getParams()
{
    return m_params;
}