#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <string_view>
#include <stdexcept>
#include <memory>

#include "ResourceManager.hpp"
#include "Scene.hpp"

namespace planets
{

    class Application
    {
    public:
        Application(int argc, char *argv[], const std::string &configPath = "config.cfg");
        ~Application();

        Application(const Application &other) = delete;
        Application &operator=(const Application &other) = delete;

        void loop();

    private:
        std::shared_ptr<spdlog::logger> m_Logger;
        std::shared_ptr<spdlog::sinks::ringbuffer_sink_mt> m_VirtualConsoleLogSink;

        GLFWwindow *m_Window;

        std::unique_ptr<ResourceManager> m_ResourceManager;
        std::string m_DataDirectory{"data"};

        struct ApplicationWindowParams
        {
            int windowWidth{640};
            int windowHeight{480};
            bool fullscreen{false};
            bool cursorEnabled{true};
        } m_WindowParams;

        struct ApplicationTimings
        {
            double lastTime{0.0};
            double currentDelta{0.0};
            double fps{0.0};
            void update(double currentTime)
            {
                currentDelta = currentTime - lastTime;
                lastTime = currentTime;
                double momentaryFps = 1.0 / currentDelta;

                // EMA
                constexpr double alpha = 0.8;
                fps = fps * alpha + momentaryFps * (1.0 - alpha);
            }
        } m_ApplicationTimings;

        struct DebugParams
        {
            bool debugConsoleActive{false};
        } m_DebugParams;

        struct ControlParams
        {
            double lastCursorPosX{0};
            double lastCursorPosY{0};
            double cursorDeltaX{0};
            double cursorDeltaY{0};
        } m_ControlParams;

        std::unique_ptr<Scene> m_CurrentScene;

        void initLogger();

        void initPlatform();

        void initImGui();

        void loadConfig(const std::string &configPath);

        void draw(double deltaTime);

        void initScene();

        void drawDebugTree(std::shared_ptr<SpatialObject> node);
        void drawDebugConsole();
        void drawImGui();

        // Actual callbacks
        void framebufferSizeCallback(int width, int height);
        void keyCallback(int key, int scancode, int action, int mods);
        void characterCallback(unsigned int codepoint);
        void cursorPositionCallback(double xpos, double ypos);
        void mouseButtonCallback(int button, int action, int mods);
        void scrollCallback(double xoffset, double yoffset);

        // Callbacks must be either static methods, or free functions. These are wrappers for the above methods.
        static void GLFW_error_callback(int error, const char *description);
        static void GLFW_framebuffer_size_callback(GLFWwindow *window, int width, int height);
        static void GLFW_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void GLFW_character_callback(GLFWwindow *window, unsigned int codepoint);
        static void GLFW_cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
        static void GLFW_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void GLFW_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    };

};