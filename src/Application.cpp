#include "Application.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ringbuffer_sink.h>

#include <simpleini/SimpleIni.h>

#include <string_view>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <algorithm>

#include "ShaderProgram.hpp"
#include "ResourceManager.hpp"
#include "Camera.hpp"
#include "StaticMeshInstance.hpp"

namespace planets
{
    Application::Application(int argc, char *argv[], const std::string &configPath)
    {
        (void)argc;
        (void)argv;
        initLogger();
        loadConfig(configPath);
        initPlatform();
        initImGui();
        
        m_ResourceManager = std::make_unique<ResourceManager>(m_DataDirectory);

        initScene();
    }

    Application::~Application()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Application::initLogger()
    {
        auto console_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
        console_sink->set_level(spdlog::level::trace);

        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("planets.log", true);
        file_sink->set_level(spdlog::level::trace);

        m_VirtualConsoleLogSink = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(1024);
        file_sink->set_level(spdlog::level::trace);

        m_Logger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{console_sink,
                                                                                file_sink,
                                                                                m_VirtualConsoleLogSink});

        m_Logger->set_level(spdlog::level::trace);

        spdlog::set_default_logger(m_Logger);
    }

    void Application::loadConfig(const std::string &configPath)
    {
        spdlog::trace("Loading config file \"{}\"", configPath);
        CSimpleIniA ini;
        // ini.SetUnicode();
        SI_Error rc = ini.LoadFile(configPath.c_str());
        if (rc < 0)
        {
            spdlog::warn("Error loading the config file. Default values will be used.");
            return;
        }

        const char *pv;
        char *end;
        int width, height;

        pv = ini.GetValue("Application", "WindowWidth", "");
        width = strtol(pv, &end, 10);
        if (strlen(pv) == 0 || *end != '\0' || width <= 0)
        {
            spdlog::warn("Config: window width not defined. Default value of 640 will be used.");
        }
        else
        {
            m_WindowParams.windowWidth = width;
        }

        pv = ini.GetValue("Application", "WindowHeight", "");
        height = strtol(pv, &end, 10);
        if (strlen(pv) == 0 || *end != '\0' || height <= 0)
        {
            spdlog::warn("Config: window height not defined. Default value of 480 will be used.");
        }
        else
        {
            m_WindowParams.windowHeight = height;
        }

        pv = ini.GetValue("Application", "WindowFullscreen", "");
        if (strlen(pv) == 0)
        {
            spdlog::warn("Config: window fullscreen mode not defined. Default value of False will be used.");
        }
        else
        {
            if (strcmp(pv, "True") == 0)
            {
                m_WindowParams.fullscreen = true;
            }
            else if (strcmp(pv, "False") == 0)
            {
                m_WindowParams.fullscreen = false;
            }
            else
            {
                spdlog::warn("Config: window fullscreen mode not defined. Default value of False will be used.");
            }
        }

        pv = ini.GetValue("Application", "DataDirectory", "");
        if (strlen(pv) == 0)
        {
            spdlog::warn("Config: data directory not defined. Default value of \"data\" will be used.");
        }
        else
        {
            m_DataDirectory = pv;
        }
    }

    void Application::loop()
    {

        while (!glfwWindowShouldClose(m_Window))
        {
            glfwSetInputMode(m_Window, GLFW_CURSOR, m_WindowParams.cursorEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

            m_ApplicationTimings.update(glfwGetTime());

            // Cursor pos
            double xpos, ypos;
            glfwGetCursorPos(m_Window, &xpos, &ypos);
            m_ControlParams.cursorDeltaX = xpos - m_ControlParams.lastCursorPosX;
            m_ControlParams.cursorDeltaY = ypos - m_ControlParams.lastCursorPosY;
            m_ControlParams.lastCursorPosX = xpos;
            m_ControlParams.lastCursorPosY = ypos;

            // Do fixed update from another thread? (synchronization?)
            m_CurrentScene->update(static_cast<float>(m_ApplicationTimings.currentDelta));

            auto cam = m_CurrentScene->getActiveCamera();

            auto pos = cam->getLocalPosition();

            glm::vec3 axis{0.0f};

            if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS)
            {
                axis.z = -1.0f;
            }
            if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS)
            {
                axis.z = 1.0f;
            }
            if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS)
            {
                axis.x = -1.0f;
            }
            if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS)
            {
                axis.x = 1.0f;
            }

            float speed = 0.03f;
            if (glfwGetKey(m_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            {
                speed = 0.1f;
            }

            if (glm::length(axis) > 0.0001f)
            {
                axis = glm::normalize(axis.x * cam->getLocalRight() + axis.z * cam->getLocalForward());

                cam->setLocalPosition(pos + axis * speed);
            }

            auto rot = cam->getLocalRotation();

            float sensitivity = 0.0005f;
            rot.x -= m_ControlParams.cursorDeltaY * sensitivity;
            rot.y -= m_ControlParams.cursorDeltaX * sensitivity;

            rot.x = std::clamp(rot.x, -M_PI_2f32, M_PI_2f32);

            cam->setLocalRotation(rot);


            
            auto suzanne = m_CurrentScene->getRoot()->getChild("Suzanne");
            
            auto sRot = suzanne->getLocalRotation();
            sRot.y += 0.005f;
            sRot.x += 0.0005f;
            suzanne->setLocalRotation(sRot);

            auto sPos = suzanne->getLocalPosition();
            sPos.y = 3.f + std::sin(m_ApplicationTimings.lastTime);
            suzanne->setLocalPosition(sPos);

            auto suzanne1 = suzanne->getChild("Suzanne1");

            auto sRot1 = suzanne1->getLocalRotation();
            sRot1.x += 0.01f;
            suzanne1->setLocalRotation(sRot1);

            auto suzanne2 = suzanne1->getChild("Suzanne2");

            auto sRot2 = suzanne2->getLocalRotation();
            sRot2.z += 0.01f;
            suzanne2->setLocalRotation(sRot2);
            

            draw(m_ApplicationTimings.currentDelta);

            glfwSwapBuffers(m_Window);
            glfwPollEvents();
        }
    }

    void Application::draw(double deltaTime)
    {
        (void)deltaTime;

        // Draw scene
        m_CurrentScene->draw(m_WindowParams.windowWidth, m_WindowParams.windowHeight);

        // Draw (debug) GUI
        drawImGui();
    }

    void Application::framebufferSizeCallback(int width, int height)
    {
        m_WindowParams.windowWidth = width;
        m_WindowParams.windowHeight = height;
    }

    void Application::keyCallback(int key, int scancode, int action, int mods)
    {
        (void)key;
        (void)scancode;
        (void)action;
        (void)mods;
        // Debug console, available always and in all contexts
        if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
        {
            m_DebugParams.debugConsoleActive = !m_DebugParams.debugConsoleActive;
        }
        if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        {
            m_WindowParams.cursorEnabled = !m_WindowParams.cursorEnabled;
        }
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_Window, 1);
        }
    }

    void Application::characterCallback(unsigned int codepoint)
    {
        (void)codepoint;
    }

    void Application::cursorPositionCallback(double xpos, double ypos)
    {
        (void)xpos;
        (void)ypos;
    }

    void Application::mouseButtonCallback(int button, int action, int mods)
    {
        (void)button;
        (void)action;
        (void)mods;
    }

    void Application::scrollCallback(double xoffset, double yoffset)
    {
        (void)xoffset;
        (void)yoffset;
    }
}