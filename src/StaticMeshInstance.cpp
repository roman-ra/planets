#include "StaticMeshInstance.hpp"

#include "SpatialObject.hpp"
#include "StaticMesh.hpp"
#include "Material.hpp"

#include "DebugUtils.hpp"

#include <memory>

#include <glm/glm.hpp>

namespace planets
{
    StaticMeshInstance::StaticMeshInstance(const std::string &name,
                                           std::shared_ptr<SpatialObject> parent,
                                           std::shared_ptr<StaticMesh> mesh,
                                           std::shared_ptr<Material> material) : SpatialObject(name, parent),
                                                                                 m_Mesh(mesh),
                                                                                 m_Material(material)
    {
    }

    void StaticMeshInstance::draw(const DrawInput &drawInput, DrawStats &drawStats)
    {
        MaterialInput matInput{
            drawInput.viewProjection,
            drawInput.viewProjection * m_LocalToWorld,
            m_LocalToWorld,
            m_WorldRotationM3x3, // For normals
            drawInput.cameraPosition,
            drawInput.time};

        drawStats.drawCalls++;
        drawStats.staticMeshes++;

        m_Material->use(matInput);
        m_Mesh->draw();
        m_Material->disable();

        SpatialObject::draw(drawInput, drawStats);
    }
}