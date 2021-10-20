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
        if (!m_objects[0].expired())
            m_objects[0].lock()->setName(name);
    }

    //! Add a component
    void TargetObject::addComponent(const std::shared_ptr<Component> &component) 
    {
        if (m_objects.size() <= 0 || component == nullptr)
            return;
        auto compName = component->getName();
        auto found = std::find_if(m_components.begin(), m_components.end(), [compName](auto element) {
            return compName.compare(element->getName()) == 0;
        });

        if (found == m_components.end())
        {
            json jComp;
            component->to_json(jComp);

            auto compoundComp = std::make_shared<CompoundComponent>(*this);
            for (int i = 0; i < m_objects.size(); ++i) {
                if (!m_objects[i].expired()) {
                    auto comp = m_objects[i].lock()->createComponent(compName);
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
                if (!object.expired())
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
                if (!object.expired())
                    object.lock()->removeComponent(name);
            }
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Remove a component by id
    bool TargetObject::removeComponent(uint64_t id)
    {
        auto found = std::find_if(m_components.begin(), m_components.end(), [&id](auto element) {
            auto compoundComponent = std::dynamic_pointer_cast<CompoundComponent>(element);
            return compoundComponent && id == compoundComponent->getInstanceId();
        });
        if (found != m_components.end())
        {
            for (auto& object : m_objects)
            {
                if (!object.expired())
                    object.lock()->removeComponent((*found)->getName());
            }
            m_components.erase(found);
            return true;
        }
        return false;
    }

    //! Update
    void TargetObject::onUpdate(float dt)
    {
        // Clear expired objects
        auto itr = m_objects.begin();
        while (itr != m_objects.end()) {
            if (itr->expired()) { itr = m_objects.erase(itr); continue; }
            ++itr;
        }
    }

    //! Check active
    bool TargetObject::isActive(bool recursive) const
    {
        if(m_objects.size() <= 0 || m_objects[0].expired())
            return false;
        return m_objects[0].lock()->isActive(recursive);
    }

    //! Enable or disable the actor
    void TargetObject::setActive(bool isActive, bool recursive)
    {
        for(auto& obj : m_objects)
        {
            if (!obj.expired())
                obj.lock()->setActive(isActive, recursive);
        }
    }

    //! Utils to collect shared components
    void TargetObject::collectSharedComponents()
    {
        m_components.clear();
        if(m_objects.size() <= 0)
            return;
        if(m_objects[0].expired()) {
            clear();
            return;
        }

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
                    auto type = comp->getType();
                    const auto& components = m_objects[i].lock()->getComponents();
                    const auto& itr = std::find_if(components.begin(), components.end(), [&type](auto elem) {
                        return elem->getType() == type;
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

        auto itr = m_objects.begin();
        while (itr != m_objects.end()) {
            if (itr->expired()) { itr = m_objects.erase(itr); continue; }
            ++itr;
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
        return m_objects.size() > 0 && !m_objects[0].expired() ? m_objects[0].lock() : nullptr;
    }

    //! Get all targets
    std::vector< std::weak_ptr<SceneObject>>& TargetObject::getAllTargets()
    {
        return m_objects;
    }

    //! Find object by id among all targets
    std::shared_ptr<SceneObject> TargetObject::findObject(uint64_t id)
    {
        auto itr = std::find_if(m_objects.begin(), m_objects.end(), [&](const auto& elem) {
            return !elem.expired() && elem.lock()->getId() == id;
        });
        return (itr != m_objects.end()) ? (*itr).lock() : nullptr;
    }

} // namespace ige::scene
