#include "StaticMesh.hpp"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>
#include <stdexcept>

#include <spdlog/spdlog.h>

namespace planets
{
    StaticMesh::StaticMesh(const std::vector<glm::vec3> &vertexPositions,
                           const std::vector<glm::vec3> &vertexNormals,
                           const std::vector<glm::vec2> &vertexUVs,
                           const std::vector<GLuint> &triangleIndices) : m_TriangleIndices(triangleIndices),
                                                                         m_IsOnGPU(false),
                                                                         m_VboId(0),
                                                                         m_VaoId(0),
                                                                         m_EboId(0)

    {
        spdlog::trace("Creating a static mesh");
        if (vertexPositions.size() == 0 || vertexNormals.size() == 0 || vertexUVs.size() == 0 || triangleIndices.size() == 0)
        {
            spdlog::error("Vertex attribute arrays and array of idices cannot be empty");
            throw std::runtime_error("Vertex attribute arrays and array of idices cannot be empty");
        }
        if (vertexPositions.size() != vertexNormals.size() || vertexPositions.size() != vertexUVs.size() || vertexNormals.size() != vertexUVs.size())
        {
            spdlog::error("Vertex attribute arrays must have equal lengths");
            throw std::runtime_error("Vertex attribute arrays must have equal lengths");
        }
        if (triangleIndices.size() % 3 != 0)
        {
            spdlog::error("Length of the array of indices must be divisible by 3");
            throw std::runtime_error("Length of the array of indices must be divisible by 3");
        }

        for (size_t i = 0; i < vertexPositions.size(); i++)
        {
            m_Vertices.emplace_back(vertexPositions[i], vertexNormals[i], vertexUVs[i]);
        }
    }

    StaticMesh::~StaticMesh()
    {
        spdlog::trace("Deleting a static mesh");
        if (m_IsOnGPU)
        {
            unloadFromGPU();
        }
    }

    void StaticMesh::uploadToGPU()
    {
        spdlog::trace("Uploading static mesh to GPU");
        if (m_IsOnGPU)
        {
            spdlog::warn("Trying to upload mesh data that has already been uploaded");
            return;
        }

        GLuint vboId, vaoId, eboId;

        glGenVertexArrays(1, &vaoId);
        if (vaoId == 0)
        {
            spdlog::error("Unable to create Vertex Array Object");
            throw std::runtime_error("Unable to create Vertex Array Object");
        }

        glGenBuffers(1, &vboId);
        if (vboId == 0)
        {
            glDeleteVertexArrays(1, &vaoId);
            spdlog::error("Unable to create Vertex Buffer Object");
            throw std::runtime_error("Unable to create Vertex Buffer Object");
        }

        glGenBuffers(1, &eboId);
        if (eboId == 0)
        {
            glDeleteVertexArrays(1, &vaoId);
            glDeleteVertexArrays(1, &vboId);
            spdlog::error("Unable to create Element Buffer Object");
            throw std::runtime_error("Unable to create Element Buffer Object");
        }

        glBindVertexArray(vaoId);

        // Upload vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vboId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices[0]) * m_Vertices.size(),
                     reinterpret_cast<void *>(&m_Vertices[0]), GL_STATIC_DRAW);

        // Upload indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_TriangleIndices[0]) * m_TriangleIndices.size(),
                     reinterpret_cast<void *>(&m_TriangleIndices[0]), GL_STATIC_DRAW);

        // Set vertex attribute pointer
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMesh::Vertex),
                              reinterpret_cast<void *>(offsetof(StaticMesh::Vertex, position)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticMesh::Vertex),
                              reinterpret_cast<void *>(offsetof(StaticMesh::Vertex, normal)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticMesh::Vertex),
                              reinterpret_cast<void *>(offsetof(StaticMesh::Vertex, uv)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        m_VaoId = vaoId;
        m_VboId = vboId;
        m_EboId = eboId;

        m_IsOnGPU = true;
    }

    void StaticMesh::unloadFromGPU()
    {
        spdlog::trace("Unloading static mesh from GPU");
        if (!m_IsOnGPU)
        {
            spdlog::warn("Trying to unload mesh data that has not been uploaded");
            return;
        }

        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteBuffers(1, &m_VboId);
        glDeleteBuffers(1, &m_EboId);

        m_VaoId = 0;
        m_VboId = 0;
        m_EboId = 0;

        m_IsOnGPU = false;
    }

    void StaticMesh::draw() const noexcept
    {
        if (!m_IsOnGPU)
        {
            spdlog::error("Trying to draw a mesh that has not been uploaded");
            return;
        }

        glBindVertexArray(m_VaoId);
        glDrawElements(GL_TRIANGLES, m_TriangleIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}