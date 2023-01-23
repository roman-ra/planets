#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <vector>

namespace planets
{
    class StaticMesh
    {
    public:
        StaticMesh(const std::vector<glm::vec3> &vertexPositions,
                   const std::vector<glm::vec3> &vertexNormals,
                   const std::vector<glm::vec2> &vertexUVs,
                   const std::vector<GLuint> &triangleIndices);
        ~StaticMesh();

        void uploadToGPU();
        void unloadFromGPU();

        void draw() const noexcept;

    private:
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec3 tangent;
            glm::vec2 uv;
            Vertex(const glm::vec3 &position,
                   const glm::vec3 &normal,
                   const glm::vec3 &tangent,
                   const glm::vec2 &uv) : position(position),
                                          normal(normal),
                                          tangent(tangent),
                                          uv(uv)
            {
            }
        };

        std::vector<StaticMesh::Vertex> m_Vertices;
        /*std::vector<glm::vec3> m_VertexPositions;
        std::vector<glm::vec3> m_VertexNormals;
        std::vector<glm::vec2> m_VertexUVs;*/
        std::vector<GLuint> m_TriangleIndices;

        bool m_IsOnGPU;

        GLuint m_VboId;
        GLuint m_VaoId;
        GLuint m_EboId;
    };
}
