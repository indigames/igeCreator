#include "core/scene/CompoundComponent.h"

#include <scene/SceneObject.h>
#include <scene/Scene.h>

namespace ige::scene
{
    //! Constructor
    CompoundComponent::CompoundComponent(SceneObject &owner)
        : Component(owner) 
    { }

    //! Destructor
    CompoundComponent::~CompoundComponent()
    { }

    //! Returns the name of the component
    std::string CompoundComponent::getName() const {
        if (m_components.size() > 0)
            return m_components[0]->getName();
        return "CompoundComponent";
    }

    //! Returns the name of the component
    Component::Type CompoundComponent::getType() const {
        if (m_components.size() > 0)
            return m_components[0]->getType();
        return Component::Type::Compound;
    }

    //! Add component
    void CompoundComponent::add(const std::shared_ptr<Component>& comp)
    {
        if(comp != nullptr)
        {
            auto itr = std::find(m_components.begin(), m_components.end(), comp);
            if (itr == m_components.end())
            {
                m_components.push_back(comp);
                collectSharedElements();
            }
        }        
    }

    //! Remove component
    bool CompoundComponent::remove(const std::shared_ptr<Component>& comp)
    {
        if (comp != nullptr)
        {
            auto itr = std::find(m_components.begin(), m_components.end(), comp);
            if (itr != m_components.end())
            {
                m_components.erase(itr);
                collectSharedElements();
                return true;
            }
        }
        return false;
    }

    //! Remove component by ID
    bool CompoundComponent::remove(uint64_t compId)
    {
        auto itr = std::find_if(m_components.begin(), m_components.end(), [&compId](auto elem) {
            return compId == elem->getInstanceId();
        });
        if (itr != m_components.end())
        {
            m_components.erase(itr);
            collectSharedElements();
            return true;
        }
        return false;
    }

    //! Clear components
    void CompoundComponent::clear()
    {
        m_json.clear();
        m_components.clear();
    }

    //! Utils to collect shared elements
    void CompoundComponent::collectSharedElements()
    {
        m_json.clear();
        if (m_components.size() == 0) return;
        m_components[0]->to_json(m_json);
        json jComp;
        for (int i = 1; i < m_components.size(); ++i)
        {
            m_components[i]->to_json(jComp);
            
            for (auto& [key, val] : m_json.items())
            {
                if (jComp.contains(key) && jComp[key] != val)
                {
                    try {
                        Vec4 vec1;
                        jComp[key].get_to<Vec4>(vec1);

                        Vec4 vec2;
                        val.get_to<Vec4>(vec2);

                        auto x = vmath_almostEqual(vec1[0], vec2[0]) ? vec1[0] : NAN;
                        auto y = vmath_almostEqual(vec1[1], vec2[1]) ? vec1[1] : NAN;
                        auto z = vmath_almostEqual(vec1[2], vec2[2]) ? vec1[2] : NAN;
                        auto w = vmath_almostEqual(vec1[3], vec2[3]) ? vec1[3] : NAN;
                        m_json[key] = Vec4(x, y, z, w);
                        continue;
                    }
                    catch (std::exception e) {}

                    try {
                        Vec3 vec1;
                        jComp[key].get_to<Vec3>(vec1);

                        Vec3 vec2;
                        val.get_to<Vec3>(vec2);

                        auto x = vmath_almostEqual(vec1[0], vec2[0]) ? vec1[0] : NAN;
                        auto y = vmath_almostEqual(vec1[1], vec2[1]) ? vec1[1] : NAN;
                        auto z = vmath_almostEqual(vec1[2], vec2[2]) ? vec1[2] : NAN;
                        m_json[key] = Vec3(x, y, z);
                        continue;
                    }
                    catch (std::exception e) {}
                    
                    try {
                        Vec2 vec1;
                        jComp[key].get_to<Vec2>(vec1);

                        Vec2 vec2;
                        val.get_to<Vec2>(vec2);

                        auto x = vmath_almostEqual(vec1[0], vec2[0]) ? vec1[0] : NAN;
                        auto y = vmath_almostEqual(vec1[1], vec2[1]) ? vec1[1] : NAN;

                        m_json[key] = Vec2(x, y);
                        continue;
                    }
                    catch (std::exception e) {}

                    m_json[key] = nullptr;
                    continue;
                }
            }
        }
    }

    //! Update json value
    void CompoundComponent::setProperty(const std::string& key, const json& val)
    {
        if (m_json.contains(key))
        {
            m_json[key] = val;
            for (auto& comp : m_components)
            {
                comp->setProperty(key, val);
            }
        }
    }

    //! Serialize
    void CompoundComponent::to_json(json& j) const
    {
        j = m_json;
    }

    //! Enable
    void CompoundComponent::onEnable()
    {
        for (auto& comp : m_components)
            comp->onEnable();
    }

    //! Disable
    void CompoundComponent::onDisable()
    {
        for (auto& comp : m_components)
            comp->onDisable();
    }

    //! Update functions
    void CompoundComponent::onUpdate(float dt)
    {
        for (auto& comp : m_components)
            comp->onUpdate(dt);
    }

    void CompoundComponent::onFixedUpdate(float dt)
    {
        for (auto& comp : m_components)
            comp->onFixedUpdate(dt);
    }

    void CompoundComponent::onLateUpdate(float dt)
    {
        for (auto& comp : m_components)
            comp->onLateUpdate(dt);
    }

    //! Render
    void CompoundComponent::onRender()
    {
        for (auto& comp : m_components)
            comp->onRender();
    }

    //! Destroyed
    void CompoundComponent::onDestroy()
    {
        for (auto& comp : m_components)
            comp->onDestroy();
    }

    //! Click
    void CompoundComponent::onClick()
    {
        for (auto& comp : m_components)
            comp->onClick();
    }

    //! Suspend
    void CompoundComponent::onSuspend()
    {
        for (auto& comp : m_components)
            comp->onSuspend();
    }

    //! Resume
    void CompoundComponent::onResume()
    {
        for (auto& comp : m_components)
            comp->onResume();
    }

} // namespace ige::scene