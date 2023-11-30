#include <Input.hpp>

#include <cstring>
#include <unordered_map>

static std::unordered_map<GLFWwindow*, em::Input*> windowInputMap;

static inline void i_setNthBit(uint32_t* flagBuffer, int bit);
static inline void i_clearNthBit(uint32_t* flagBuffer, int bit);
static inline int i_getNthBit(const uint32_t* flagBuffer, int bit);

using namespace em;

Input::Input(GLFWwindow* window) :
    m_window(window),
    m_prevMousePosition(0.0f),
    m_mousePosition(0.0f),
    m_mouseDelta(0.0f),
    m_mouseScroll(0.0f),
    m_mouseMoved(false),
    m_mouseScrolled(false)
{
    clearAllBuffers();

    registerCallbacks();

    windowInputMap[window] = this;
}

bool Input::isKeyPressed(int key)
{
    return i_getNthBit(m_keyPressed, key);
}

bool Input::isKeyReleased(int key)
{
    return i_getNthBit(m_keyReleased, key);
}

bool Input::isKeyHeld(int key)
{
    return i_getNthBit(m_keyHeld, key);
}

bool Input::isKeyRepeating(int key)
{
    return i_getNthBit(m_keyRepeating, key);
}

bool Input::isMousePressed(int button)
{
    return i_getNthBit(m_mousePressed, button);
}

bool Input::isMouseReleased(int button)
{
    return i_getNthBit(m_mouseReleased, button);
}

bool Input::isMouseHeld(int button)
{
    return i_getNthBit(m_mouseHeld, button);
}

glm::vec2 Input::getMousePosition()
{
    return m_mousePosition;
}

glm::vec2 Input::getMouseDelta()
{
    return m_mouseDelta;
}

bool Input::mouseMoved()
{
    return m_mouseMoved;
}

bool Input::mouseScrolled()
{
    return m_mouseScrolled;
}

glm::vec2 Input::getMouseScroll()
{
    return m_mouseScroll;
}

void Input::update()
{
    clearIntermittentBuffers();

    m_mouseDelta = glm::vec2(0.0f);
    m_mouseMoved = false;

    m_mouseScroll = glm::vec2(0.0f);
    m_mouseScrolled = false;
}

void Input::registerCallbacks()
{
    glfwSetKeyCallback(m_window, keyCallback);
    glfwSetScrollCallback(m_window, mouseScrollCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, mousePositionCallback);
}

void Input::clearAllBuffers()
{
    memset(m_keyPressed, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyReleased, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyHeld, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyRepeating, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);

    memset(m_mousePressed, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
    memset(m_mouseReleased, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
    memset(m_mouseHeld, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
}

void Input::clearIntermittentBuffers()
{
    memset(m_keyPressed, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyReleased, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);

    memset(m_mousePressed, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
    memset(m_mouseReleased, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(windowInputMap.find(window) == windowInputMap.end())
        return;

    Input* input = windowInputMap[window];

    if(action == GLFW_PRESS)
    {
        i_setNthBit(input->m_keyPressed, key);
        i_setNthBit(input->m_keyHeld, key);
    }
    else if(action == GLFW_RELEASE)
    {
        i_setNthBit(input->m_keyReleased, key);
        i_clearNthBit(input->m_keyHeld, key);
        i_clearNthBit(input->m_keyRepeating, key);
    }
    else if(action == GLFW_REPEAT)
    {
        i_setNthBit(input->m_keyHeld, key);
        i_setNthBit(input->m_keyRepeating, key);
    }
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if(windowInputMap.find(window) == windowInputMap.end())
        return;

    Input* input = windowInputMap[window];

    if(action == GLFW_PRESS)
    {
        i_setNthBit(input->m_mousePressed, button);
        i_setNthBit(input->m_mouseHeld, button);
    }
    else if(action == GLFW_RELEASE)
    {
        i_setNthBit(input->m_mouseReleased, button);
        i_clearNthBit(input->m_mouseHeld, button);
    }
}

void Input::mousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if(windowInputMap.find(window) == windowInputMap.end())
        return;

    Input* input = windowInputMap[window];

    input->m_prevMousePosition = input->m_mousePosition;
    input->m_mousePosition = glm::vec2(xpos, ypos);
    input->m_mouseDelta = input->m_mousePosition - input->m_prevMousePosition;
    input->m_mouseMoved = true;
}

void Input::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(windowInputMap.find(window) == windowInputMap.end())
        return;

    Input* input = windowInputMap[window];

    input->m_mouseScroll = glm::vec2(xoffset, yoffset);
    input->m_mouseScrolled = true;
}

static inline void i_setNthBit(uint32_t* flagBuffer, int bit)
{
    flagBuffer[bit / 32] |= (1 << (bit % 32));
}

static inline void i_clearNthBit(uint32_t* flagBuffer, int bit)
{
    flagBuffer[bit / 32] &= ~(1 << (bit % 32));
}

static inline int i_getNthBit(const uint32_t* flagBuffer, int bit)
{
    return (flagBuffer[bit / 32] >> (bit % 32)) & 1;
}