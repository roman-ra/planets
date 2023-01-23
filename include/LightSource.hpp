#pragma once

#include "SpatialObject.hpp"

#include <glm/glm.hpp>

namespace planets
{
    class LightSource : public SpatialObject
    {
    public:
        struct LightMaterialInput
        {
            glm::vec3 worldPosition;

            
        };
        LightSource() = delete;
        LightSource(const std::string &name, std::shared_ptr<SpatialObject> parent);

    private:
        float m_Intensity;
        glm::vec3 m_Color;
    };
}
