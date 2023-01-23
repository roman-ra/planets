#include "Material.hpp"

#include "ShaderProgram.hpp"

#include <memory>

namespace planets
{

    Material::Material(std::shared_ptr<ShaderProgram> shaderProgram) : m_ShaderProgram(shaderProgram)
    {
    }

    Material::~Material()
    {
    }

    void Material::use(const MaterialInput &materialInput) const
    {
        // Activate shader program
        m_ShaderProgram->use();

        // Set built-in uniforms
        m_ShaderProgram->setMatrix4f("modelToClipSpace", materialInput.modelToClipSpace);
        m_ShaderProgram->setMatrix4f("modelToWorldSpace", materialInput.modelToWorldSpace);
        m_ShaderProgram->setMatrix3f("modelToWorldSpace_Normal", materialInput.modelToWorldSpace_Normal);
        m_ShaderProgram->setVector3f("cameraWorldPosition", materialInput.cameraPosition);
        m_ShaderProgram->setFloat("time", materialInput.time);
    }

    void Material::disable() const
    {
    }

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    StandardMaterial::StandardMaterial(std::shared_ptr<ShaderProgram> shaderProgram,
                                       GLint flags) : Material(shaderProgram), m_Flags(flags)
    {
        // All uniform locations have already been retrieved by the constructor of the ShaderProgram
    }

    StandardMaterial::~StandardMaterial()
    {
    }

    void StandardMaterial::use(const MaterialInput &materialInput) const
    {
        Material::use(materialInput);

        m_ShaderProgram->setInt("materialFlags", m_Flags);

        m_ShaderProgram->setVector3f("diffuseColor", m_DiffuseColor);

        if (m_Flags & StandardMaterialFlags::HAS_DIFFUSE_MAP)
        {
            m_DiffuseMap->bind(0);
            m_ShaderProgram->setInt("diffuseMap", 0);
        }

        if (m_Flags & StandardMaterialFlags::HAS_ROUGHNESS_MAP)
        {
            m_RoughnessMap->bind(1);
            m_ShaderProgram->setInt("roughnessMap", 1);
        }

        if (m_Flags & StandardMaterialFlags::HAS_NORMAL_MAP)
        {
            m_NormalMap->bind(2);
            m_ShaderProgram->setInt("normalMap", 2);
        }

        if (m_Flags & StandardMaterialFlags::HAS_METALNESS_MAP)
        {
            m_MetalnessMap->bind(3);
            m_ShaderProgram->setInt("metalnessMap", 3);
        }

        if (m_Flags & StandardMaterialFlags::HAS_EMISSION_MAP)
        {
            m_EmissionMap->bind(4);
            m_ShaderProgram->setInt("emissionMap", 4);
        }

        if (m_Flags & StandardMaterialFlags::HAS_AO_MAP)
        {
            m_AoMap->bind(5);
            m_ShaderProgram->setInt("aoMap", 5);
        }
    }

    void StandardMaterial::disable() const
    {
        if (m_Flags & StandardMaterialFlags::HAS_DIFFUSE_MAP)
        {
            m_DiffuseMap->unbind(0);
        }
        if (m_Flags & StandardMaterialFlags::HAS_ROUGHNESS_MAP)
        {
            m_RoughnessMap->unbind(1);
        }
        if (m_Flags & StandardMaterialFlags::HAS_NORMAL_MAP)
        {
            m_NormalMap->unbind(2);
        }
        if (m_Flags & StandardMaterialFlags::HAS_METALNESS_MAP)
        {
            m_MetalnessMap->unbind(3);
        }
        if (m_Flags & StandardMaterialFlags::HAS_EMISSION_MAP)
        {
            m_EmissionMap->unbind(4);
        }
        if (m_Flags & StandardMaterialFlags::HAS_AO_MAP)
        {
            m_AoMap->unbind(5);
        }
    }
}