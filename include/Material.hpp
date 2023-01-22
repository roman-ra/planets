#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "ShaderProgram.hpp"
#include "Texture2D.hpp"

#include <memory>

namespace planets
{
    struct DrawInput
    {
        const glm::mat4 &viewProjection;
        const glm::vec3 &cameraPosition;
        const GLfloat time;
    };

    struct MaterialInput
    {
        const glm::mat4 &viewProjection;
        const glm::mat4 &modelToClipSpace;
        const glm::mat4 &modelToWorldSpace;
        const glm::mat3 &modelToWorldSpace_Normal;
        const glm::vec3 &cameraPosition;
        const GLfloat time;
    };

    class Material
    {
    public:
        Material(std::shared_ptr<ShaderProgram> shaderProgram);
        virtual ~Material();

        virtual void use(const MaterialInput &materialInput) const;
        virtual void disable() const;

    protected:
        std::shared_ptr<ShaderProgram> m_ShaderProgram;
    };

    class StandardMaterial : public Material
    {
    public:
        StandardMaterial(std::shared_ptr<ShaderProgram> shaderProgram,
                         std::shared_ptr<Texture2D> diffuseTexture);
        virtual ~StandardMaterial() override;

        virtual void use(const MaterialInput &materialInput) const override;
        virtual void disable() const override;

    private:
        std::shared_ptr<Texture2D> m_DiffuseTexture;
    };
}
