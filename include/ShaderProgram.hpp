#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

namespace planets
{

    class ShaderProgram
    {
    public:
        ShaderProgram() = delete;
        ShaderProgram(const std::string &vertexSource, const std::string &fragmentSource);
        ~ShaderProgram();

        void use() const noexcept;

        void setMatrix4f(const char *name, const glm::mat4 &matrix);
        void setMatrix3f(const char *name, const glm::mat3 &matrix);
        void setMatrix2f(const char *name, const glm::mat2 &matrix);
        void setVector4f(const char *name, const glm::vec4 &vector);
        void setVector3f(const char *name, const glm::vec3 &vector);
        void setVector2f(const char *name, const glm::vec2 &vector);
        void setFloat(const char *name, GLfloat value);
        void setInt(const char *name, GLint value);

    private:
        GLuint m_ProgramId;
        // Uniforms

        std::unordered_map<std::string, GLint> m_UniformLocations;

        inline bool uniformExists(const char *name)
        {
            return (m_UniformLocations.find(name) != m_UniformLocations.end());
        }

        void getUniformLocations();
    };

}