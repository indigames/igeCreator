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
        return "CompoundComponent";
    }

    //! Get containee name
    std::string CompoundComponent::getContainName()
    {
        if (m_components.size() > 0)
            return m_components[0]->getName();
        return "";
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
            return true;
        }
        return false;
    }

    void CompoundComponent::clear()
    {
        m_components.clear();
    }


    //! Enable/Disable
    bool CompoundComponent::isEnabled() const
    {
        if(m_components.empty())
            return false;
        return m_components[0]->isEnabled();
    }

    void CompoundComponent::setEnabled(bool enable)
    {
        for(auto& comp: m_components)
        {
            if(comp) comp->setEnabled(enable);
        }
    }

} // namespace ige::scene