#include "Application.hpp"

#include <glad/gl.h>
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
#include <string>

#include "ShaderProgram.hpp"
#include "ResourceManager.hpp"
#include "Camera.hpp"
#include "StaticMeshInstance.hpp"

namespace planets
{

    void Application::initScene()
    {
        std::unique_ptr<Scene> scene = std::make_unique<Scene>();

        // Load resources
        auto defaultShader = m_ResourceManager->loadShaderProgram("default",
                                                                  "shaders/test_vert.glsl",
                                                                  "shaders/test_frag.glsl");
        auto testMaterial = m_ResourceManager->createMaterial("test", defaultShader);
        /*
        m_ResourceManager->loadStaticMesh("Suzanne",
                                          "models/Suzanne.obj")
            ->uploadToGPU();

        m_ResourceManager->loadStaticMesh("Sponza",
                                          "models/CrytekSponza/sponza.obj")
            ->uploadToGPU();

        auto testMaterial = m_ResourceManager->createMaterial("test", defaultShader);

        auto tex = m_ResourceManager->loadTexture2DFromPNG("Bricks", "models/textures/red_brick_03_diff_2k.png");

        auto brickMat = m_ResourceManager->createStandardMaterial("BricksMat", tex);

        */

        auto testMaterial2 = m_ResourceManager->createStandardMaterial("test2", m_ResourceManager->getTexture2D("NOTEXTURE"));

        size_t counter{0};
        auto SponzaMeshes = m_ResourceManager->loadStaticMesh("Sponza", "models/sponza_separated.obj");
        for (auto &[mesh, material] : SponzaMeshes)
        {
            mesh->uploadToGPU();
            scene->addObject(std::make_shared<StaticMeshInstance>("Sponza." + std::to_string(counter++),
                                                                  scene->getRoot(),
                                                                  mesh,
                                                                  material));
        }

        // Add Suzanne
        /*auto suzanne = scene->addObject(std::make_shared<StaticMeshInstance>("Suzanne",
                                                                             scene->getRoot(),
                                                                             m_ResourceManager->getStaticMesh("Suzanne"),
                                                                             brickMat));
        suzanne->setLocalPosition({0, 2, 0});

        auto suzanne1 = suzanne->addChild(std::make_shared<StaticMeshInstance>("Suzanne1",
                                                                               suzanne,
                                                                               m_ResourceManager->getStaticMesh("Suzanne"),
                                                                               brickMat));
        suzanne1->setLocalScale({0.4, 0.4, 0.4});
        suzanne1->setLocalPosition({2, 0, 0});

        auto sponza = scene->addObject(std::make_shared<StaticMeshInstance>("Sponza",
                                                                            scene->getRoot(),
                                                                            m_ResourceManager->getStaticMesh("Sponza"),
                                                                            brickMat));
        */

        // Add camera
        auto cam = scene->addObject(std::make_shared<Camera>("Camera", scene->getRoot()));
        scene->setActiveCamera(std::dynamic_pointer_cast<Camera>(cam));
        cam->setLocalPosition({0, 1.8, 5});

        m_CurrentScene = std::move(scene);
    }

}