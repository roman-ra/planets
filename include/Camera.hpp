#pragma once

#include "SpatialObject.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace planets
{

    class Camera : public SpatialObject
    {
    public:
        Camera() = delete;
        Camera(const std::string &name, std::shared_ptr<SpatialObject> parent) : SpatialObject(name, parent), m_FieldOfView(M_PI / 3), m_AspectRatio(1.f)
        {
            recalculateProjectionMatrix();
        }

        void setAspectRatio(float aspectRatio)
        {
            m_AspectRatio = aspectRatio;
            recalculateProjectionMatrix();
        }

        void setFieldOfView(float radians)
        {
            m_FieldOfView = radians;
            recalculateProjectionMatrix();
        }

        glm::mat4 getViewProjectionMatrix() { return m_Projection * m_WorldToLocal; }
        const glm::mat4 &getViewMatrix() { return m_WorldToLocal; }
        const glm::mat4 &getProjectionMatrix() { return m_Projection; }

    private:
        float m_FieldOfView;
        float m_AspectRatio;

        glm::mat4 m_Projection;
        glm::mat4 m_ViewProjection;

        void recalculateProjectionMatrix()
        {
            m_Projection = glm::perspective(m_FieldOfView, m_AspectRatio, 0.1f, 1000.f);
        }
    };

}