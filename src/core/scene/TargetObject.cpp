#include "core/scene/TargetObject.h"
#include "core/scene/CompoundComponent.h"

#include <scene/Scene.h>

namespace ige::scene
{    
    //! Constructor
    TargetObject::TargetObject(Scene* scene)
        : SceneObject::SceneObject(scene, 0)
    {       
    }

    //! Destructor
    TargetObject::~TargetObject()
    {

    }

    //! Get ID
    uint64_t TargetObject::getId() const 
    {
        if(m_objects.size() == 1)
            return m_objects[0].lock()->getId();
        return 0;
    }

    //! Get UUID
    std::string TargetObject::getUUID() const 
    {
        if (m_objects.size() == 0)
            return "";
        else if(m_objects.size() == 1)
            return m_objects[0].lock()->getUUID();
        return "Multiple Values";
    }

    //! Get Name
    std::string TargetObject::getName() const 
    {
        if (m_objects.size() == 0)
            return "";
        else if(m_objects.size() == 1)
            return m_objects[0].lock()->getName();
        return "Multiple Values";
    }

    void TargetObject::setName(const std::string& name)
    {
        int size = m_objects.size();
        for (int i = 0; i < size; i++)
        {
            m_objects[0].lock()->setName(name);
        }
    }

    //! Add a component
    void TargetObject::addComponent(const std::shared_ptr<Component> &component) 
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&component](auto element) {
            return component->getName().compare(element->getName()) == 0;
        });

        if (found == m_components.end())
        {
            auto compoundComp = std::make_shared<CompoundComponent>(*this);
            json jComp;
            component->to_json(jComp);
            for(int i = 0; i < m_objects.size(); ++i)
            {
                auto obj = m_objects[i];
                if (!obj.expired())
                {
                    auto comp = obj.lock()->createComponent(component->getName());
                    comp->from_json(jComp);
                    compoundComp->add(comp);
                }
            }
            compoundComp->setDirty();
            m_components.push_back(compoundComp);
        }
    }

    //! Remove a component
    bool TargetObject::removeComponent(const std::shared_ptr<Component> &component) 
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&component](auto element) {
            auto compoundComponent = std::dynamic_pointer_cast<CompoundComponent>(element);
            return compoundComponent && component->getName().compare(compoundComponent->getName()) == 0;
        });

        if (found != m_components.end())
        {
            for(auto& object: m_objects)
            {
                object.lock()->removeComponent(component->getName());
            }
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Remove a component by name
    bool TargetObject::removeComponent(const std::string& name)
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&name](auto element) {
            auto compoundComponent = std::dynamic_pointer_cast<CompoundComponent>(element);
            return compoundComponent && name.compare(compoundComponent->getName()) == 0;
        });

        if (found != m_components.end())
        {
            for (auto& object : m_objects)
            {
                object.lock()->removeComponent(name);
            }
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Check active
    bool TargetObject::isActive() const
    {
        if(m_objects.size() <= 0)
            return false;
        return m_objects[0].lock()->isActive();
    }

    //! Enable or disable the actor
    void TargetObject::setActive(bool isActive)
    {
        for(auto& obj : m_objects)
        {
            obj.lock()->setActive(isActive);
        }
    }
    
    //! Utils to collect shared components
    void TargetObject::collectSharedComponents()
    {
        m_components.clear();
        if(m_objects.size() < 0)
            return;

        for(auto comp: m_objects[0].lock()->getComponents())
        {
            if (comp == nullptr) continue;

            if(comp && (m_objects.size() == 1 || comp->canMultiEdit()))
            {
                auto compoundComp = std::make_shared<CompoundComponent>(*this);
                compoundComp->add(comp);

                auto shoudAdd = true;
                for (int i = 1; i < m_objects.size(); ++i)
                {
                    auto name = comp->getName();
                    const auto& components = m_objects[i].lock()->getComponents();
                    const auto& itr = std::find_if(components.begin(), components.end(), [&name](auto elem) {
                        return name.compare(elem->getName()) == 0;
                    });

                    if (itr != components.end())
                    {
                        compoundComp->add(*itr);
                    }
                    else
                    {
                        shoudAdd = false;
                        break;
                    }
                }

                if (shoudAdd)
                {
                    m_components.push_back(compoundComp);
                }
            }
        }
    }
    
    //! Add object
    void TargetObject::add(std::shared_ptr<SceneObject> object)
    {
        if(object != nullptr)
        {
            object->setSelected(true);
            auto itr = std::find_if(m_objects.begin(), m_objects.end(), [&](auto& elem) {
                return !elem.expired() && elem.lock()->getId() == object->getId();
            });
            if (itr == m_objects.end())
            {
                m_objects.push_back(object);
                collectSharedComponents();
            }
        }
    }

    //! Remove object
    void TargetObject::remove(std::shared_ptr<SceneObject> object)
    {
        if(object != nullptr)
        {
            auto itr = std::find_if(m_objects.begin(), m_objects.end(), [&](auto& elem) {
                return !elem.expired() && elem.lock()->getId() == object->getId();
            });
            if(itr != m_objects.end())
            {
                object->setSelected(false);
                m_objects.erase(itr);
                collectSharedComponents();
            }            
        }
    }

    //! Clear object
    void TargetObject::clear()
    {
        for (auto& object : m_objects)
        {
            if (!object.expired())
            {
                object.lock()->setSelected(false);
            }
        }
        m_components.clear();
        m_objects.clear();
    }

    //! Get first target
    std::shared_ptr<SceneObject> TargetObject::getFirstTarget()
    {
        return m_objects.size() > 0 ? m_objects[0].lock() : nullptr;
    }

    //! Get all targets
    std::vector< std::weak_ptr<SceneObject>>& TargetObject::getAllTargets()
    {
        return m_objects;
    }

} // namespace ige::scene
