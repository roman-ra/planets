#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace planets
{
    class Texture2D
    {
    public:
        enum class TextureDataFormat
        {
            RGB8,
            RGBA8,
            RGB_FLOAT,
            RGBA_FLOAT
        };

        Texture2D() = delete;
        Texture2D(GLsizei width, GLsizei height, const void *dataPtr, Texture2D::TextureDataFormat format);
        ~Texture2D();
        
        void bind(GLint unit) const noexcept
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, m_TextureId);
        }

        void unbind(GLint unit) const noexcept
        {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, 0);
        }


    private:
        GLuint m_TextureId;
    };
}