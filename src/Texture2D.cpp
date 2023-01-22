#include "Texture2D.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace planets
{
    Texture2D::Texture2D(GLsizei width, GLsizei height, const void *dataPtr, Texture2D::TextureDataFormat format)
    {
        if (format != TextureDataFormat::RGB8 && format != TextureDataFormat::RGBA8)
        {
            spdlog::error("Only RGB8 and RGBA8 textures are supported");
            throw std::runtime_error("Texture format not supported");
        }

        GLuint textureId;
        glGenTextures(1, &textureId);

        // TODO: add error handling

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        switch (format)
        {
        case TextureDataFormat::RGB8:
            spdlog::trace("Uploading {}x{} RGB8 texture data to GPU", width, height);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataPtr);
            break;
        case TextureDataFormat::RGBA8:
            spdlog::trace("Uploading {}x{} RGBA8 texture data to GPU", width, height);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataPtr);
            break;
        default:
            break;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_TextureId = textureId;
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_TextureId);
    }

}