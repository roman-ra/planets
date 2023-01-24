#include "ResourceManager.hpp"
#include "ShaderProgram.hpp"
#include "StaticMesh.hpp"
#include "Material.hpp"
#include "Texture2D.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <unordered_map>
#include <memory>
#include <stdexcept>

#include <spdlog/spdlog.h>

#include <tinyobjloader/tiny_obj_loader.h>

#include <stb/stb_image.h>

#include <fstream>
#include <sstream>

namespace planets
{
    ResourceManager::ResourceManager(const std::string &dataDirectory) : m_DataDirectory(dataDirectory)
    {
        // Load the standard feature-rich shader
        loadShaderProgram("Standard", "shaders/Standard_vert.glsl", "shaders/Standard_frag.glsl");
        loadTexture2DFromPNG("NOTEXTURE", "textures/NOTEXTURE.png");
    }

    ResourceManager::~ResourceManager()
    {
    }

    std::shared_ptr<ShaderProgram> ResourceManager::loadShaderProgram(const std::string &name,
                                                                      const std::string &vertexShaderSourcePath,
                                                                      const std::string &fragmentShaderSourcePath)
    {
        std::string vertexShaderSourcePathFull = makePath(vertexShaderSourcePath);
        std::string fragmentShaderSourcePathFull = makePath(fragmentShaderSourcePath);

        spdlog::trace("Loading shader program \"{}\" with vertex shader source \"{}\" and fragment shader source \"{}\"",
                      name, vertexShaderSourcePathFull, fragmentShaderSourcePathFull);
        if (m_ShaderPrograms.find(name) != m_ShaderPrograms.end())
        {
            spdlog::warn("Shader program \"{}\" already exists and will be replaced", name);
        }

        std::string vertexShaderSource;
        std::string fragmentShaderSource;

        std::ifstream vertexShaderSourceStream(vertexShaderSourcePathFull, std::ios::in);
        if (vertexShaderSourceStream.is_open())
        {
            std::stringstream buffer;
            buffer << vertexShaderSourceStream.rdbuf();
            vertexShaderSource = buffer.str();
            vertexShaderSourceStream.close();
        }
        else
        {
            spdlog::error("Unable to open vertex shader source file at \"{}\"", vertexShaderSourcePathFull);
            throw std::runtime_error("Unable to open vertex shader source file");
        }

        std::ifstream fragmentShaderSourceStream(fragmentShaderSourcePathFull, std::ios::in);
        if (fragmentShaderSourceStream.is_open())
        {
            std::stringstream buffer;
            buffer << fragmentShaderSourceStream.rdbuf();
            fragmentShaderSource = buffer.str();
            fragmentShaderSourceStream.close();
        }
        else
        {
            spdlog::error("Unable to open fragment shader source file at \"{}\"", fragmentShaderSourcePathFull);
            throw std::runtime_error("Unable to open fragment shader source file");
        }

        std::shared_ptr<ShaderProgram> prog = std::make_shared<ShaderProgram>(vertexShaderSource, fragmentShaderSource);

        m_ShaderPrograms[name] = prog;

        return prog;
    }

    std::shared_ptr<ShaderProgram> ResourceManager::getShaderProgram(const std::string &name) const
    {
        auto it = m_ShaderPrograms.find(name);
        if (it == m_ShaderPrograms.end())
        {
            spdlog::error("Unable to find shader program \"{}\"", name);
            throw std::runtime_error("Unable to find shader program");
        }
        return it->second;
    }

    std::shared_ptr<Material> ResourceManager::createMaterial(const std::string &name, std::shared_ptr<ShaderProgram> shaderProgram)
    {
        spdlog::trace("Creating material \"{}\"", name);
        if (m_Materials.find(name) != m_Materials.end())
        {
            spdlog::warn("Material \"{}\" already exists and will be replaced", name);
        }
        std::shared_ptr<Material> mat = std::make_shared<Material>(shaderProgram);
        m_Materials[name] = mat;
        return mat;
    }

    std::shared_ptr<Material> ResourceManager::getMaterial(const std::string &name)
    {
        auto it = m_Materials.find(name);
        if (it == m_Materials.end())
        {
            spdlog::error("Unable to find material \"{}\"", name);
            throw std::runtime_error("Unable to find material");
        }
        return it->second;
    }

    std::shared_ptr<Texture2D> ResourceManager::loadTexture2DFromPNG(const std::string &name,
                                                                     const std::string &path)
    {
        std::string fullPath = makePath(path);
        spdlog::trace("Loading a 2D texture \"{}\" from PNG file \"{}\"", name, fullPath);
        if (m_Textures2D.find(name) != m_Textures2D.end())
        {
            spdlog::warn("2D texture \"{}\" already exists and will be replaced", name);
        }

        int width{0}, height{0}, numChannels{0};
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(fullPath.c_str(), &width, &height, &numChannels, 0);
        // If we couldn't load the texture, we just return the Source-like emo checkerboard
        if (data == NULL)
        {
            spdlog::warn("Unable to load 2D texture from \"{}\"", fullPath);
            return getTexture2D("NOTEXTURE");
        }

        Texture2D::TextureDataFormat format;
        if (numChannels == 1)
        {
            format = Texture2D::TextureDataFormat::R8;
        }
        else if (numChannels == 3)
        {
            format = Texture2D::TextureDataFormat::RGB8;
        }
        else if (numChannels == 4)
        {
            format = Texture2D::TextureDataFormat::RGBA8;
        }
        else
        {
            spdlog::warn("Unsupported image format");
            stbi_image_free(data);
            throw std::runtime_error("Unsupported image format");
        }

        std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(static_cast<GLsizei>(width),
                                                                     static_cast<GLsizei>(height),
                                                                     reinterpret_cast<void *>(data),
                                                                     format);

        m_Textures2D[name] = tex;
        stbi_image_free(data);
        return tex;
    }

    std::shared_ptr<Texture2D> ResourceManager::getTexture2D(const std::string &name)
    {
        auto it = m_Textures2D.find(name);
        if (it == m_Textures2D.end())
        {
            spdlog::error("Unable to find 2D texture \"{}\"", name);
            throw std::runtime_error("Unable to find 2D texture");
        }
        return it->second;
    }

    std::shared_ptr<Material> ResourceManager::createStandardMaterial(const std::string &name,
                                                                      GLint flags)
    {
        spdlog::trace("Creating Standard material \"{}\"", name);
        if (m_Materials.find(name) != m_Materials.end())
        {
            spdlog::warn("Material \"{}\" already exists and will be replaced", name);
        }
        std::shared_ptr<Material> mat = std::make_shared<StandardMaterial>(getShaderProgram("Standard"),
                                                                           flags);
        m_Materials[name] = mat;
        return mat;
    }

    std::vector<std::pair<std::shared_ptr<StaticMesh>,
                          std::shared_ptr<Material>>>
    ResourceManager::loadStaticMesh(const std::string &name,
                                    const std::string &objPath)
    {
        std::string fullPath = makePath(objPath);
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig readerConfig;
        readerConfig.mtl_search_path = makePath("/models/");
        readerConfig.triangulate = true;

        spdlog::trace("Loading static mesh \"{}\" from OBJ file \"{}\"", name, fullPath);

        if (!reader.ParseFromFile(fullPath, readerConfig))
        {
            if (!reader.Error().empty())
            {
                spdlog::error("Unable to load OBJ \"{}\". TinyObjReader: {}", fullPath, reader.Error());
            }
            throw std::runtime_error("Unable to load OBJ");
        }

        if (!reader.Warning().empty())
        {
            spdlog::warn("TinyObjReader: {}", reader.Warning());
        }

        auto &attrib = reader.GetAttrib();
        auto &shapes = reader.GetShapes();
        auto &materials = reader.GetMaterials();

        // Load material(s)
        std::vector<std::shared_ptr<Material>> createdMaterials;

        for (size_t i = 0; i < materials.size(); i++)
        {
            const auto &objMaterial = materials[i];
            std::shared_ptr<StandardMaterial> material = std::dynamic_pointer_cast<StandardMaterial>(createStandardMaterial(materials[i].name, 0));

            // Load diffuse texture if exists
            if (objMaterial.diffuse_texname.size() > 0)
            {
                spdlog::trace("Loading diffuse map {}", objMaterial.diffuse_texname);
                auto tex = loadTexture2DFromPNG(objMaterial.diffuse_texname, objMaterial.diffuse_texname);
                material->setDiffuseMap(tex);
            }

            glm::vec3 diffuseColor{objMaterial.diffuse[0],
                                   objMaterial.diffuse[1],
                                   objMaterial.diffuse[2]};
            // To avoid getting black objects when diffuse color is missing in the MTL
            if (glm::length(diffuseColor) > 0.01f)
            {
                material->setDiffuseColor(diffuseColor);
            }

            // Load normal map if exists
            if (objMaterial.normal_texname.size() > 0)
            {
                spdlog::trace("Loading normal map {}", objMaterial.normal_texname);
                auto tex = loadTexture2DFromPNG(objMaterial.normal_texname, objMaterial.normal_texname);
                material->setNormalMap(tex);
            }

            // Load roughness map if exists
            if (objMaterial.roughness_texname.size() > 0)
            {
                spdlog::trace("Loading rougness map {}", objMaterial.roughness_texname);
                auto tex = loadTexture2DFromPNG(objMaterial.roughness_texname, objMaterial.roughness_texname);
                material->setRoughnessMap(tex);
            }

            // Load metalness map if exists
            if (objMaterial.metallic_texname.size() > 0)
            {
                spdlog::trace("Loading metalness map {}", objMaterial.metallic_texname);
                auto tex = loadTexture2DFromPNG(objMaterial.metallic_texname, objMaterial.metallic_texname);
                material->setMetalnessMap(tex);
            }

            spdlog::info("Material flags: {}", material->getFlags());

            createdMaterials.push_back(material);
        }
        spdlog::trace("Loaded {} materials defined in the MTL", createdMaterials.size());

        // Create static mesh(es)
        std::vector<std::shared_ptr<StaticMesh>> createdMeshes;
        std::vector<int> materialIndices;
        std::vector<glm::vec3> vertexPositions;
        std::vector<glm::vec3> vertexNormals;
        std::vector<glm::vec2> vertexUVs;
        std::vector<GLuint> triangleIndices;
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

                for (size_t v = 0; v < fv; v++)
                {
                    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    vertexPositions.emplace_back(vx, vy, vz);

                    if (idx.normal_index >= 0)
                    {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        vertexNormals.emplace_back(nx, ny, nz);
                    }
                    else
                    {
                        vertexNormals.emplace_back(0.0f);
                    }

                    if (idx.texcoord_index >= 0)
                    {
                        tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                        vertexUVs.emplace_back(tx, ty);
                    }
                    else
                    {
                        vertexUVs.emplace_back(0.0f);
                    }
                    triangleIndices.emplace_back(index_offset + v);
                }
                index_offset += fv;
            }

            // Pick the correct matrial for this (sub)mesh
            materialIndices.push_back(shapes[s].mesh.material_ids[0]);

            std::string submeshName = name + '.' + shapes[s].name;

            std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(vertexPositions, vertexNormals, vertexUVs, triangleIndices);
            if (m_StaticMeshes.find(submeshName) != m_StaticMeshes.end())
            {
                spdlog::warn("Static mesh \"{}\" already exists and will be replaced", name);
            }
            m_StaticMeshes[submeshName] = mesh;
            createdMeshes.push_back(mesh);

            vertexPositions.clear();
            vertexNormals.clear();
            vertexUVs.clear();
            triangleIndices.clear();
        }

        spdlog::trace("Loaded {} static meshes", createdMeshes.size());

        std::vector<std::pair<std::shared_ptr<StaticMesh>, std::shared_ptr<Material>>> allMeshesWithMats;
        for (size_t i = 0; i < createdMeshes.size(); i++)
        {
            allMeshesWithMats.push_back(std::make_pair(createdMeshes[i], createdMaterials[materialIndices[i]]));
        }

        return allMeshesWithMats;
    }

    std::shared_ptr<StaticMesh> ResourceManager::getStaticMesh(const std::string &name) const
    {
        auto it = m_StaticMeshes.find(name);
        if (it == m_StaticMeshes.end())
        {
            spdlog::error("Unable to find static mesh \"{}\"", name);
            throw std::runtime_error("Unable to find static mesh");
        }
        return it->second;
    }

    void ResourceManager::reloadStandardShader()
    {
        auto newProgram = loadShaderProgram("Standard", "shaders/Standard_vert.glsl", "shaders/Standard_frag.glsl");
        for (auto &m : m_Materials)
        {
            auto sm = std::dynamic_pointer_cast<StandardMaterial>(m.second);
            if (sm)
            {
                sm->replaceProgram(newProgram);
            }
        }
    }
}