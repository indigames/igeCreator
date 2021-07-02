#include "core/scene/CompoundComponent.h"

#include <scene/SceneObject.h>
#include <scene/Scene.h>

namespace ige::scene
{

    //! Constructor
    CompoundComponent::CompoundComponent(SceneObject &owner)
        : Component(owner) 
    {

    }

    //! Destructor
    CompoundComponent::~CompoundComponent()
    {

    }

    //! Returns the name of the component
    std::string CompoundComponent::getName() const
    {
        if (m_components.size() > 0)
            return m_components[0]->getName();
        return "CompoundComponent";
    }

    //! Add/remove/clear components
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

    void CompoundComponent::clear()
    {
        m_json.clear();
        m_components.clear();
    }


    //! Utils to collect shared elements
    void CompoundComponent::collectSharedElements()
    {
        m_json.clear();
        if (m_components.size() == 0)
            return;

        m_components[0]->to_json(m_json);

        json jComp;
        for (int i = 1; i < m_components.size(); ++i)
        {
            m_components[i]->to_json(jComp);
            
            for (auto& [key, val] : m_json.items())
            {
                if (jComp.contains(key) && jComp[key] != val)
                {
                    m_json[key] = nullptr;
                    continue;
                }
            }
        }
    }

    //! Update json value
    void CompoundComponent::setJson(const json& val)
    {
        if (m_components.size() > 0)
        {
            for (auto& comp : m_components)
            {
                comp->from_json(val);
            }
            m_json = val;
        }
        else
        {
            m_json.clear();
        }
    }

    //! Update json value
    void CompoundComponent::setJson(const std::string& key, const json& val)
    {
        if (m_json.contains(key))
        {
            m_json[key] = val;
        }
    }

} // namespace ige::scene