#include "Scene.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/gtx/string_cast.hpp>
#include <spdlog/spdlog.h>

#include "SpatialObject.hpp"
#include "Camera.hpp"
#include "Material.hpp"

#include <memory>

namespace planets
{
    Scene::Scene()
    {
        spdlog::trace("Creating scene");
        // Create root node
        m_Root = std::make_shared<SpatialObject>("ROOT", nullptr);
    }

    Scene::~Scene()
    {
        spdlog::trace("Destroying scene");
    }

    std::shared_ptr<SpatialObject> Scene::addObject(std::shared_ptr<SpatialObject> object)
    {
        m_Root->addChild(object);
        return object;
    }

    void Scene::setActiveCamera(std::shared_ptr<Camera> camera)
    {
        m_ActiveCamera = camera;
    }

    void Scene::update(float deltaTime)
    {
        (void)deltaTime;
    }

    void Scene::fixedUpdate()
    {
    }

    void Scene::draw(int viewportWidth, int viewportHeight)
    {
        m_ActiveCamera->setAspectRatio(static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight));
        glm::mat4 viewProjection = m_ActiveCamera->getViewProjectionMatrix();

        DrawInput drawInput{
            viewProjection,
            m_ActiveCamera->getGlobalPosition(),
            static_cast<float>(glfwGetTime())
            
        };

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glViewport(0, 0, viewportWidth, viewportHeight);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        drawStats.reset();

        // Recursively draw the tree (DFS)
        m_Root->draw(drawInput, drawStats);
    }
}