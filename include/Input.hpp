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

        bool isKeyPressed(int key) const; // Initial press
        bool isKeyReleased(int key) const; // Initial release
        bool isKeyHeld(int key) const; // Pressed and held
        bool isKeyRepeating(int key) const; // Held and repeating

        bool isMousePressed(int button) const; // Initial press
        bool isMouseReleased(int button) const; // Initial release
        bool isMouseHeld(int button) const; // Pressed and held
        glm::vec2 getMousePosition() const; // Get mouse position
        glm::vec2 getMouseDelta() const; // Get mouse delta
        bool mouseMoved() const; // Has the mouse just moved?
        bool mouseScrolled() const; // Has the mouse just scrolled?
        glm::vec2 getMouseScroll() const; // Get mouse scroll

        void setLockMouse(bool lock) const; // Lock the mouse to the window
        bool isMouseLocked() const; // Is the mouse locked to the window?
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
        mutable bool m_mouseLocked;

        void update();
        void registerCallbacks();
        void clearAllBuffers();
        void clearIntermittentBuffers();

        friend class VisualizerApp;
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void mousePositionCallback(GLFWwindow* window, double xpos, double ypos);
        static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    };
}