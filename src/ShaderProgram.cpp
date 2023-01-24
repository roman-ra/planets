#include "ShaderProgram.hpp"

#include <spdlog/spdlog.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <stdexcept>

namespace planets
{

    ShaderProgram::ShaderProgram(const std::string &vertexSource, const std::string &fragmentSource)
    {
        // TODO: Insert builtins

        spdlog::trace("Creating a shader program from source");
        spdlog::trace("Creating shader objects");
        GLuint vertexShaderId{glCreateShader(GL_VERTEX_SHADER)};
        GLuint fragmentShaderId{glCreateShader(GL_FRAGMENT_SHADER)};

        if (vertexShaderId == 0)
        {
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            spdlog::error("Error creating vertex shader");
            throw std::runtime_error("Error creating vertex shader");
        }
        if (fragmentShaderId == 0)
        {
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            spdlog::error("Error creating fragment shader");
            throw std::runtime_error("Error creating fragment shader");
        }

        const char *vertexSourcePtr = vertexSource.c_str();
        glShaderSource(vertexShaderId, 1, &vertexSourcePtr, NULL);
        spdlog::trace("Compiling vertex shader");
        glCompileShader(vertexShaderId);

        GLint compileStatus{GL_FALSE};
        GLint infoLogLength{0};

        glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &compileStatus);
        glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (compileStatus == GL_FALSE)
        {
            spdlog::error("Error compiling vertex shader");
            std::vector<char> infoLog(infoLogLength + 1);
            glGetShaderInfoLog(vertexShaderId, infoLogLength, NULL, &infoLog[0]);
            spdlog::error("[GLSL]: {}", &infoLog[0]);
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            throw std::runtime_error("Error compiling vertex shader");
        }
        spdlog::trace("Successfully compiled vertex shader");

        compileStatus = GL_FALSE;
        infoLogLength = 0;

        const char *fragmentSourcePtr = fragmentSource.c_str();
        glShaderSource(fragmentShaderId, 1, &fragmentSourcePtr, NULL);
        spdlog::trace("Compiling fragment shader");
        glCompileShader(fragmentShaderId);

        glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &compileStatus);
        glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (compileStatus == GL_FALSE)
        {
            spdlog::error("Error compiling fragment shader");
            std::vector<char> infoLog(infoLogLength + 1);
            glGetShaderInfoLog(fragmentShaderId, infoLogLength, NULL, &infoLog[0]);
            spdlog::error("[GLSL]: {}", &infoLog[0]);
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            throw std::runtime_error("Error compiling fragment shader");
        }
        spdlog::trace("Successfully compiled fragment shader");

        spdlog::trace("Creating program");
        GLuint programId{glCreateProgram()};
        if (programId == 0)
        {
            spdlog::error("Error creating program");
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            throw std::runtime_error("Error creating program");
        }

        spdlog::trace("Linking program");
        glAttachShader(programId, vertexShaderId);
        glAttachShader(programId, fragmentShaderId);
        glLinkProgram(programId);

        GLint linkStatus{GL_FALSE};
        infoLogLength = 0;
        glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

        if (linkStatus == GL_FALSE)
        {
            spdlog::error("Error linking program");
            std::vector<char> infoLog(infoLogLength + 1);
            glGetProgramInfoLog(programId, infoLogLength, NULL, &infoLog[0]);
            spdlog::error("[GLSL]: {}", &infoLog[0]);
            glDetachShader(programId, vertexShaderId);
            glDetachShader(programId, fragmentShaderId);
            glDeleteShader(vertexShaderId);
            glDeleteShader(fragmentShaderId);
            glDeleteProgram(programId);
            throw std::runtime_error("Error linking program");
        }
        spdlog::trace("Successfully linked program");

        glDetachShader(programId, vertexShaderId);
        glDetachShader(programId, fragmentShaderId);

        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);

        m_ProgramId = programId;

        getUniformLocations();
    }

    ShaderProgram::~ShaderProgram()
    {
        spdlog::trace("Deleting a shader program");
        glDeleteProgram(m_ProgramId);
    }

    void ShaderProgram::use() const noexcept
    {
        glUseProgram(m_ProgramId);
    }

    void ShaderProgram::getUniformLocations()
    {
        GLint numUniforms{0};
        glGetProgramiv(m_ProgramId, GL_ACTIVE_UNIFORMS, &numUniforms);
        spdlog::trace("Retrieving locations of {} active uniforms", numUniforms);

        for (GLint i = 0; i < numUniforms; i++)
        {
            GLint size;
            GLenum type;
            const GLsizei maxNameLen = 256;
            GLchar name[maxNameLen];
            GLsizei length;

            glGetActiveUniform(m_ProgramId, (GLuint)i, maxNameLen, &length, &size, &type, name);
            GLint location = glGetUniformLocation(m_ProgramId, name);
            m_UniformLocations[name] = location;

            spdlog::trace("Uniform \"{}\" is at location \"{}\"", name, location);
        }
    }

    void ShaderProgram::setMatrix4f(const char *name, const glm::mat4 &matrix)
    {
        if (uniformExists(name))
            glUniformMatrix4fv(m_UniformLocations[name], 1, GL_FALSE, &matrix[0][0]);
    }

    void ShaderProgram::setMatrix3f(const char *name, const glm::mat3 &matrix)
    {
        if (uniformExists(name))
            glUniformMatrix3fv(m_UniformLocations[name], 1, GL_FALSE, &matrix[0][0]);
    }

    void ShaderProgram::setMatrix2f(const char *name, const glm::mat2 &matrix)
    {
        if (uniformExists(name))
            glUniformMatrix2fv(m_UniformLocations[name], 1, GL_FALSE, &matrix[0][0]);
    }

    void ShaderProgram::setVector4f(const char *name, const glm::vec4 &vector)
    {
        if (uniformExists(name))
            glUniform4fv(m_UniformLocations[name], 1, &vector[0]);
    }

    void ShaderProgram::setVector3f(const char *name, const glm::vec3 &vector)
    {
        if (uniformExists(name))
            glUniform3fv(m_UniformLocations[name], 1, &vector[0]);
    }

    void ShaderProgram::setVector2f(const char *name, const glm::vec2 &vector)
    {
        if (uniformExists(name))
            glUniform2fv(m_UniformLocations[name], 1, &vector[0]);
    }

    void ShaderProgram::setFloat(const char *name, GLfloat value)
    {
        if (uniformExists(name))
            glUniform1f(m_UniformLocations[name], value);
    }

    void ShaderProgram::setInt(const char *name, GLint value)
    {
        if (uniformExists(name))
            glUniform1i(m_UniformLocations[name], value);
    }
}