#pragma once

#include "ShaderProgram.hpp"
#include "StaticMesh.hpp"
#include "Material.hpp"
#include "Texture2D.hpp"

#include <unordered_map>
#include <memory>
#include <string>

namespace planets
{

    class ResourceManager
    {
    public:
        ResourceManager(const std::string &dataDirectory);
        ~ResourceManager();

        std::shared_ptr<ShaderProgram> loadShaderProgram(const std::string &name,
                                                         const std::string &vertexShaderSourcePath,
                                                         const std::string &fragmentShaderSourcePath);
        std::shared_ptr<ShaderProgram> getShaderProgram(const std::string &name) const;

        std::shared_ptr<Material> createMaterial(const std::string &name,
                                                 std::shared_ptr<ShaderProgram> shaderProgram);
        std::shared_ptr<Material> createStandardMaterial(const std::string &name,
                                                         std::shared_ptr<Texture2D> diffuseTexture);
        std::shared_ptr<Material> getMaterial(const std::string &name);

        std::shared_ptr<Texture2D> loadTexture2DFromPNG(const std::string &name,
                                                        const std::string &path);
        std::shared_ptr<Texture2D> getTexture2D(const std::string &name);

        /*
        Returns a vector of meshes and their corresponding materials
        */
        std::vector<std::pair<std::shared_ptr<StaticMesh>,
                              std::shared_ptr<Material>>>
        loadStaticMesh(const std::string &name,
                       const std::string &objPath);
        std::shared_ptr<StaticMesh> getStaticMesh(const std::string &name) const;

    private:
        std::string m_DataDirectory;

        std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> m_ShaderPrograms;
        std::unordered_map<std::string, std::shared_ptr<Material>> m_Materials;
        std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_Textures2D;
        std::unordered_map<std::string, std::shared_ptr<StaticMesh>> m_StaticMeshes;

        std::string makePath(const std::string &relativePath) { return m_DataDirectory + '/' + relativePath; }
    };

}
