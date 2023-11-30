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
    memset(m_keyPressed, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyReleased, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyHeld, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);
    memset(m_keyRepeating, 0, sizeof(uint32_t) * INT_KEY_FLAGS_COUNT);

    memset(m_mousePressed, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
    memset(m_mouseReleased, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);
    memset(m_mouseHeld, 0, sizeof(uint32_t) * INT_MOUSE_FLAGS_COUNT);

    windowInputMap[window] = this;
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