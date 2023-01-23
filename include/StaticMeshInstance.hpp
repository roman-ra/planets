#pragma once

#include "SpatialObject.hpp"
#include "StaticMesh.hpp"
#include "ShaderProgram.hpp"
#include "Material.hpp"

#include "DebugUtils.hpp"

#include <memory>

#include <glm/glm.hpp>

namespace planets
{
    class StaticMeshInstance : public SpatialObject
    {
    public:
        StaticMeshInstance(const std::string &name,
                           std::shared_ptr<SpatialObject> parent,
                           std::shared_ptr<StaticMesh> mesh,
                           std::shared_ptr<Material> material);

        virtual void draw(const DrawInput &drawInput, DrawStats &drawStats) override;

    private:
        std::shared_ptr<StaticMesh> m_Mesh;
        std::shared_ptr<Material> m_Material;

    };
}