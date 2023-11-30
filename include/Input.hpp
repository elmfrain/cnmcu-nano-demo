#pragma once 

#include <GLFWInclude.hpp>
#include <glm/glm.hpp>
#include <inttypes.h>

namespace em
{
    class Input
    {
    public:
        Input(GLFWwindow* window);

        bool isKeyPressed(int key); // Initial press
        bool isKeyReleased(int key); // Initial release
        bool isKeyHeld(int key); // Pressed and held
        bool isKeyRepeating(int key); // Held and repeating

        bool isMousePressed(int button); // Initial press
        bool isMouseReleased(int button); // Initial release
        bool isMouseHeld(int button); // Pressed and held
        glm::vec2 getMousePosition(); // Get mouse position
        glm::vec2 getMouseDelta(); // Get mouse delta
        bool mouseMoved(); // Has the mouse just moved?
        bool mouseScrolled(); // Has the mouse just scrolled?
        glm::vec2 getMouseScroll(); // Get mouse scroll
    private:
        static const int INT_KEY_FLAGS_COUNT = 88;
        static const int INT_MOUSE_FLAGS_COUNT = 1;

        GLFWwindow* m_window;
        uint32_t m_keyPressed[INT_KEY_FLAGS_COUNT];
        uint32_t m_keyReleased[INT_KEY_FLAGS_COUNT];
        uint32_t m_keyHeld[INT_KEY_FLAGS_COUNT];
        uint32_t m_keyRepeating[INT_KEY_FLAGS_COUNT];

        uint32_t m_mousePressed[INT_MOUSE_FLAGS_COUNT];
        uint32_t m_mouseReleased[INT_MOUSE_FLAGS_COUNT];
        uint32_t m_mouseHeld[INT_MOUSE_FLAGS_COUNT];

        glm::vec2 m_prevMousePosition;
        glm::vec2 m_mousePosition;
        glm::vec2 m_mouseDelta;
        glm::vec2 m_mouseScroll;
        bool m_mouseMoved;
        bool m_mouseScrolled;

        void update();

        friend class VisualizerApp;
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
        static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    };
}