#include "Application.hpp"
#include "StaticMeshInstance.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <spdlog/spdlog.h>

#include <simpleini/SimpleIni.h>

#include <string_view>
#include <stdexcept>
#include <stack>

#include <cstdlib>
#include <cstring>

namespace planets
{

    void Application::initPlatform()
    {
        if (!glfwInit())
        {
            spdlog::critical("Failed to initialize GLFW");
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 8);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        m_Window = glfwCreateWindow(m_WindowParams.windowWidth, m_WindowParams.windowHeight, "Planets",
                                    (m_WindowParams.fullscreen ? glfwGetPrimaryMonitor() : NULL),
                                    NULL);

        if (m_Window == NULL)
        {
            spdlog::critical("Failed to create window");
            throw std::runtime_error("Failed to create window");
        }

        glfwMakeContextCurrent(m_Window);

        int version = gladLoadGL();
        if (version == 0)
        {
            spdlog::critical("Failed to create window");
            throw std::runtime_error("Failed to initialize OpenGL context");
        }

        /*if (GLAD_VERSION_MAJOR(version) != 4 || GLAD_VERSION_MINOR(version) != 6)
        {
            spdlog::critical("Failed to initalize OpenGL 4.6");
            throw std::runtime_error("Failed to initalize OpenGL 4.6");
        }

        spdlog::info("Initialized OpenGL {}.{}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));*/

        m_ApplicationTimings.lastTime = glfwGetTime();

        // Register callbacks
        glfwSetWindowUserPointer(m_Window, static_cast<void *>(this));
        glfwSetErrorCallback(Application::GLFW_error_callback);
        glfwSetFramebufferSizeCallback(m_Window, Application::GLFW_framebuffer_size_callback);
        glfwSetKeyCallback(m_Window, Application::GLFW_key_callback);
        glfwSetCharCallback(m_Window, Application::GLFW_character_callback);
        glfwSetCursorPosCallback(m_Window, Application::GLFW_cursor_position_callback);
        glfwSetMouseButtonCallback(m_Window, Application::GLFW_mouse_button_callback);
        glfwSetScrollCallback(m_Window, Application::GLFW_scroll_callback);
    }

    void Application::initImGui()
    {
        spdlog::trace("Initializing ImGui");
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        const char *glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void Application::drawDebugTree(std::shared_ptr<SpatialObject> node)
    {
        ImGui::SetNextItemOpen(true);
        if (ImGui::TreeNode(node->getName().c_str()))
        {
            glm::vec3 localPos = node->getLocalPosition();
            glm::vec3 localRot = node->getLocalRotation();
            glm::vec3 localScale = node->getLocalScale();

            ImGui::PushStyleColor(ImGuiCol_Text, 0xffefe166);
            ImGui::Text("Pos: %.1f %.1f %.1f", localPos.x, localPos.y, localPos.z);
            ImGui::Text("Rot: %.1f %.1f %.1f", localRot.x, localRot.y, localRot.z);
            ImGui::Text("Scale: %.1f %.1f %.1f", localScale.x, localScale.y, localScale.z);
            ImGui::PopStyleColor();

            auto children = node->getChildren();
            for (auto it = children.begin(); it != children.end(); it++)
            {
                drawDebugTree(it->second);
            }
            ImGui::TreePop();
        }
    }

    void Application::drawDebugConsole()
    {
        // bool sd = true;
        // ImGui::ShowDemoWindow(&sd);

        ImGui::Begin("Scene Tree", NULL);

        static size_t count = 0;
        ImGui::Text("Spawn new Suzanne");
        static glm::vec3 pos{0.0f};
        static glm::vec3 color{1.0f};

        ImGui::SliderFloat("X", &pos.x, -30.f, 30.f);
        ImGui::SliderFloat("Y", &pos.y, -30.f, 30.f);
        ImGui::SliderFloat("Z", &pos.z, -30.f, 30.f);
        ImGui::ColorPicker3("Color", &color[0]);

        if (ImGui::Button("Spawn"))
        {
            auto mtl = m_ResourceManager->createStandardMaterial("NewSuzanne" + std::to_string(count++), 0);
            (std::dynamic_pointer_cast<StandardMaterial>(mtl))->setDiffuseColor(color);
            auto suzanne = m_CurrentScene->addObject(std::make_shared<StaticMeshInstance>("NewSuzanne" + std::to_string(count++),
                                                                                          m_CurrentScene->getRoot(),
                                                                                          m_ResourceManager->getStaticMesh("Suzanne.Suzanne"),
                                                                                          mtl));
            suzanne->setLocalPosition(pos);
        }

        drawDebugTree(m_CurrentScene->getRoot());

        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Debug Console", NULL, ImGuiWindowFlags_NoMove);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
        // window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
        ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), true, window_flags);

        auto messages = m_VirtualConsoleLogSink->last_formatted(1024);

        for (size_t i = 0; i < messages.size(); i++)
            ImGui::Text(messages[i].c_str());

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndChild();

        ImGui::End();
    }

    void Application::drawImGui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // bool show_demo_window = true;
        //  ImGui::ShowDemoWindow(&show_demo_window);

        // bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Begin("Runtime Stats");
        ImGui::Text("FPS: %.1lf (%.1lf ms)", m_ApplicationTimings.fps, m_ApplicationTimings.currentDelta * 1000);
        ImGui::Text("Static meshes: %d", m_CurrentScene->drawStats.staticMeshes);
        ImGui::Text("Lights: %d", m_CurrentScene->drawStats.lights);
        ImGui::Text("Draw calls: %d", m_CurrentScene->drawStats.drawCalls);
        if (ImGui::Button("Reload Standard shader"))
        {
            try
            {
                m_ResourceManager->reloadStandardShader();
            }
            catch (std::exception &e)
            {
            }
        }
        ImGui::End();

        if (m_DebugParams.debugConsoleActive)
        {
            drawDebugConsole();
        }

        ImGui::Render();

        glViewport(0, 0, m_WindowParams.windowWidth, m_WindowParams.windowHeight);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void Application::GLFW_error_callback(int error, const char *description)
    {
        spdlog::error("GLFW[{}]; {}", error, description);
    }

    void Application::GLFW_framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        Application *appInstance = static_cast<Application *>(glfwGetWindowUserPointer(window));
        appInstance->framebufferSizeCallback(width, height);
    }

    void Application::GLFW_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Application *appInstance = static_cast<Application *>(glfwGetWindowUserPointer(window));
        appInstance->keyCallback(key, scancode, action, mods);
    }

    void Application::GLFW_character_callback(GLFWwindow *window, unsigned int codepoint)
    {
        Application *appInstance = static_cast<Application *>(glfwGetWindowUserPointer(window));
        appInstance->characterCallback(codepoint);
    }

    void Application::GLFW_cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
    {
        Application *appInstance = static_cast<Application *>(glfwGetWindowUserPointer(window));
        appInstance->cursorPositionCallback(xpos, ypos);
    }

    void Application::GLFW_mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        Application *appInstance = static_cast<Application *>(glfwGetWindowUserPointer(window));
        appInstance->mouseButtonCallback(button, action, mods);
    }

    void Application::GLFW_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        Application *appInstance = static_cast<Application *>(glfwGetWindowUserPointer(window));
        appInstance->scrollCallback(xoffset, yoffset);
    }
}