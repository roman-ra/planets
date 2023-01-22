#pragma once

#include "SpatialObject.hpp"
#include "Camera.hpp"

#include <memory>

namespace planets
{

    class Scene
    {
    public:
        Scene();
        ~Scene();

        std::shared_ptr<SpatialObject> addObject(std::shared_ptr<SpatialObject> object);

        std::shared_ptr<Camera> getActiveCamera() { return m_ActiveCamera; }
        void setActiveCamera(std::shared_ptr<Camera> camera);

        std::shared_ptr<SpatialObject> getRoot() { return m_Root; }

        void update(float deltaTime);
        void fixedUpdate();
        void draw(int viewportWidth, int viewportHeight);

    private:
        std::shared_ptr<SpatialObject> m_Root;
        std::shared_ptr<Camera> m_ActiveCamera;
    };
}