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
                                       std::shared_ptr<Texture2D> diffuseTexture) : Material(shaderProgram),
                                                                                    m_DiffuseTexture(diffuseTexture)
    {
        // Diffuse texture sampler location has already been retrieved by the constructor of the superclass
    }

    StandardMaterial::~StandardMaterial()
    {
    }

    void StandardMaterial::use(const MaterialInput &materialInput) const
    {
        Material::use(materialInput);
        m_DiffuseTexture->bind(0);
        //m_ShaderProgram->setInt("diffuseTexture", 0);
    }

    void StandardMaterial::disable() const
    {
        m_DiffuseTexture->unbind(0);
    }
}