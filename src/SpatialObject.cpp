#include "SpatialObject.hpp"
#include "Material.hpp"

#include "DebugUtils.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <memory>
#include <unordered_map>
#include <string>
#include <stdexcept>

namespace planets
{
    SpatialObject::SpatialObject(const std::string &name, std::shared_ptr<SpatialObject> parent) noexcept : m_Name(name), m_Parent(parent),
                                                                                                            m_LocalPosition(0.f),
                                                                                                            m_LocalRotation(0.f),
                                                                                                            m_LocalScale(1.f)
    {
        setLocalRotation(m_LocalRotation);
        recalculateAllMatrices();
    }

    SpatialObject::~SpatialObject()
    {
    }

    std::shared_ptr<SpatialObject> SpatialObject::addChild(std::shared_ptr<SpatialObject> object)
    {
        if (m_Children.find(object->m_Name) != m_Children.end())
        {
            spdlog::error("Trying to add a child object with the same name \"{}\"", object->m_Name);
            return object;
        }
        m_Children[object->m_Name] = object;
        object->recalculateWorldMatrices();
        return object;
    }

    bool SpatialObject::hasChild(const std::string &name) const noexcept
    {
        return m_Children.find(name) != m_Children.end();
    }

    std::shared_ptr<SpatialObject> SpatialObject::getChild(const std::string &name) const noexcept
    {
        auto it = m_Children.find(name);
        if (it == m_Children.end())
        {
            spdlog::error("Unable to get child \"{}\" of object \"{}\"", name, m_Name);
            return std::shared_ptr<SpatialObject>{nullptr};
        }
        return it->second;
    }

    void SpatialObject::setLocalPosition(const glm::vec3 &localPosition) noexcept
    {
        m_LocalPosition = localPosition;
        recalculateAllMatrices();
    }

    void SpatialObject::setLocalRotation(const glm::vec3 &localRotation) noexcept
    {
        m_LocalRotation = localRotation;
        m_LocalRotationM3x3 = glm::eulerAngleYXZ(m_LocalRotation.y,
                                                 m_LocalRotation.x,
                                                 m_LocalRotation.z);

        m_LocalRotationM = m_LocalRotationM3x3;
        m_LocalRotationM[3].w = 1.f;

        // Extract local direction vectors
        m_LocalRight = m_LocalRotationM3x3[0];
        m_LocalUp = m_LocalRotationM3x3[1];
        m_LocalForward = m_LocalRotationM3x3[2];
        recalculateAllMatrices();
    }

    void SpatialObject::setLocalScale(const glm::vec3 &localScale) noexcept
    {
        m_LocalScale = localScale;
        recalculateAllMatrices();
    }

    void SpatialObject::recalculateLocalMatrices() noexcept
    {
        m_LocalToParent = glm::translate(m_LocalPosition) * m_LocalRotationM * glm::scale(m_LocalScale);
        m_ParentToLocal = glm::inverse(m_LocalToParent);
    }

    void SpatialObject::recalculateWorldMatrices() noexcept
    {
        m_LocalToWorld = m_LocalToParent;
        m_WorldRotationM3x3 = m_LocalRotationM3x3;

        SpatialObject *parent = m_Parent.get();
        // Go up the tree until we find the root
        while (parent != nullptr)
        {
            m_LocalToWorld = parent->m_LocalToParent * m_LocalToWorld;
            m_WorldRotationM3x3 = parent->m_LocalRotationM3x3 * m_WorldRotationM3x3;
            parent = parent->m_Parent.get();
        }

        m_WorldToLocal = glm::inverse(m_LocalToWorld);

        for (auto it = m_Children.begin(); it != m_Children.end(); it++)
        {
            it->second->recalculateWorldMatrices();
        }
    }

    void SpatialObject::update(float deltaT)
    {
        (void)deltaT;
    }

    void SpatialObject::fixedUpdate()
    {
    }

    void SpatialObject::draw(const DrawInput &drawInput, DrawStats &drawStats)
    {
        // Don't draw self (since it's just an empty object) but draw children
        for (auto it = m_Children.begin(); it != m_Children.end(); it++)
        {
            it->second->draw(drawInput, drawStats);
        }
    }
}