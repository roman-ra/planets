#pragma once

#include <glad/glad.h>
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
        const glm::vec3 &cameraDirection;
        const GLfloat time;
    };

    struct MaterialInput
    {
        const glm::mat4 &viewProjection;
        const glm::mat4 &modelToClipSpace;
        const glm::mat4 &modelToWorldSpace;
        const glm::mat3 &modelToWorldSpace_Normal;
        const glm::vec3 &cameraPosition;
        const glm::vec3 &cameraDirection;
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
        enum StandardMaterialFlags : GLint
        {
            HAS_DIFFUSE_MAP = 1 << 0,
            HAS_ROUGHNESS_MAP = 1 << 1,
            HAS_NORMAL_MAP = 1 << 2,
            HAS_METALNESS_MAP = 1 << 3,
            HAS_EMISSION_MAP = 1 << 4,
            HAS_AO_MAP = 1 << 5
        };

        StandardMaterial(std::shared_ptr<ShaderProgram> shaderProgram,
                         GLint flags);
        virtual ~StandardMaterial() override;

        virtual void use(const MaterialInput &materialInput) const override;
        virtual void disable() const override;

        GLint getFlags() const
        {
            return m_Flags;
        }

        void setFlags(GLint flags)
        {
            m_Flags = flags;
        }

        void setDiffuseMap(std::shared_ptr<Texture2D> diffuseMap)
        {
            m_Flags |= HAS_DIFFUSE_MAP;
            m_DiffuseMap = diffuseMap;
        }

        void setRoughnessMap(std::shared_ptr<Texture2D> roughnessMap)
        {
            m_Flags |= HAS_ROUGHNESS_MAP;
            m_RoughnessMap = roughnessMap;
        }

        void setNormalMap(std::shared_ptr<Texture2D> normalMap)
        {
            m_Flags |= HAS_NORMAL_MAP;
            m_NormalMap = normalMap;
        }

        void setMetalnessMap(std::shared_ptr<Texture2D> metalnessMap)
        {
            m_Flags |= HAS_METALNESS_MAP;
            m_MetalnessMap = metalnessMap;
        }

        void setEmissionMap(std::shared_ptr<Texture2D> emissionMap)
        {
            m_Flags |= HAS_EMISSION_MAP;
            m_EmissionMap = emissionMap;
        }

        void setAoMap(std::shared_ptr<Texture2D> aoMap)
        {
            m_Flags |= HAS_AO_MAP;
            m_AoMap = aoMap;
        }

        void setDiffuseColor(const glm::vec3 &diffuseColor)
        {
            m_DiffuseColor = diffuseColor;
        }

        void setRoughness(GLfloat roughness)
        {
            m_Roughness = roughness;
        }

        void setMetalness(GLfloat metalness)
        {
            m_Metalness = metalness;
        }    

        void setEmissionColor(const glm::vec3 &emissionColor)
        {
            m_EmissionColor = emissionColor;
        }
        
        void replaceProgram(std::shared_ptr<ShaderProgram> newProgram)
        {
            m_ShaderProgram = newProgram;
        }
    private:
        GLint m_Flags{0};

        glm::vec3 m_DiffuseColor{1.f, 1.f, 1.f};
        GLfloat m_Roughness{0.5};
        GLfloat m_Metalness{0};
        glm::vec3 m_EmissionColor{0.f, 0.f, 0.f};

        std::shared_ptr<Texture2D> m_DiffuseMap;
        std::shared_ptr<Texture2D> m_RoughnessMap;
        std::shared_ptr<Texture2D> m_NormalMap;
        std::shared_ptr<Texture2D> m_MetalnessMap;
        std::shared_ptr<Texture2D> m_EmissionMap;
        std::shared_ptr<Texture2D> m_AoMap;
    };
}
