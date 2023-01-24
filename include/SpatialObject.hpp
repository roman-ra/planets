#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <unordered_map>
#include <string>
#include <string_view>

#include "Material.hpp"
#include "DebugUtils.hpp"

namespace planets
{

    class SpatialObject
    {
    public:
        SpatialObject() = delete;

        SpatialObject(const std::string &name, std::shared_ptr<SpatialObject> parent) noexcept;

        virtual ~SpatialObject();

        const std::string &getName() const { return m_Name; }
        std::shared_ptr<SpatialObject> addChild(std::shared_ptr<SpatialObject> object);
        bool hasChild(const std::string &name) const noexcept;
        std::shared_ptr<SpatialObject> getChild(const std::string &name) const noexcept;
        auto getChildren() const { return m_Children; }

        const glm::vec3 &getLocalRight() const { return m_LocalRight; }
        const glm::vec3 &getLocalUp() const { return m_LocalUp; }
        const glm::vec3 &getLocalForward() const { return m_LocalForward; }

        glm::vec3 getGlobalPosition() { return glm::vec3(m_LocalToWorld[3]); }
        const glm::mat3 &getGlobalRotation() { return m_WorldRotationM3x3; }

        const glm::vec3 &getLocalPosition() const { return m_LocalPosition; }
        const glm::vec3 &getLocalRotation() const { return m_LocalRotation; }
        const glm::vec3 &getLocalScale() const { return m_LocalScale; }
        void setLocalPosition(const glm::vec3 &localPosition) noexcept;
        void setLocalRotation(const glm::vec3 &localRotation) noexcept;
        void setLocalScale(const glm::vec3 &localScale) noexcept;

        virtual void update(float deltaT);
        virtual void fixedUpdate();
        

        virtual void draw(const DrawInput &drawInput, DrawStats &drawStats);

    private:
        std::string m_Name;

        std::shared_ptr<SpatialObject> m_Parent;
        std::unordered_map<std::string, std::shared_ptr<SpatialObject>> m_Children;

    protected:
        // Local transformation matrices
        glm::mat4 m_LocalToParent;
        glm::mat4 m_ParentToLocal;
        // World transformation matrices
        glm::mat4 m_LocalToWorld;
        glm::mat4 m_WorldToLocal;
        // Cached to avoid expensive (re-)computations
        glm::mat4 m_LocalRotationM;
        glm::mat3 m_LocalRotationM3x3;
        glm::mat3 m_WorldRotationM3x3;

        glm::vec3 m_LocalPosition;
        glm::vec3 m_LocalRotation; // Euler
        glm::vec3 m_LocalScale;

        glm::vec3 m_LocalRight;
        glm::vec3 m_LocalUp;
        glm::vec3 m_LocalForward;

        void recalculateLocalMatrices() noexcept;
        void recalculateWorldMatrices() noexcept;
        void recalculateAllMatrices()
        {
            recalculateLocalMatrices();
            recalculateWorldMatrices();
        }
    };

}