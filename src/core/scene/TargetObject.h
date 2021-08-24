#pragma once
#include <memory>

#include <scene/SceneObject.h>

namespace ige::scene
{
    class Scene;

    /**
    * TargetObject represents a single/group of selected object(s)
    */
    class TargetObject : public SceneObject
    {
    public:
        //! Constructor
        TargetObject(Scene* scene);

        //! Destructor
        virtual ~TargetObject();

        //! Get ID
        virtual uint64_t getId() const override;

        //! Get UUID
        virtual std::string getUUID() const override;

        //! Get Name
        virtual std::string getName() const override;

        //! Set Name 
        virtual void setName(const std::string& name) override;

        //! Add a component
        virtual void addComponent(const std::shared_ptr<Component> &component) override;

        //! Remove a component
        virtual bool removeComponent(const std::shared_ptr<Component> &component) override;

        //! Remove a component by name
        virtual bool removeComponent(const std::string& name) override;

        //! Enable or disable the actor
        void setActive(bool isActive) override;

        //! Check active
        bool isActive() const override;

        //! Serialize
        virtual void to_json(json &j) override {}

        //! Deserialize
        virtual void from_json(const json &j) override {}

        //! Add/remove/clear object
        void add(std::shared_ptr<SceneObject> object);
        void remove(std::shared_ptr<SceneObject> object);
        void clear();

        //! Check if the targets is empty
        inline bool empty() const { return m_objects.empty(); }

        //! Get the number of targets
        inline size_t size() const { return m_objects.size(); }
        
        //! Get first target
        std::shared_ptr<SceneObject> getFirstTarget();

        //! Get all targets
        std::vector<std::weak_ptr<SceneObject>>& getAllTargets();

        //! override
        virtual bool isPrefab() const override {
            return !empty() && m_objects[0].lock()->isPrefab();
        }

        virtual std::string getPrefabId() override { 
            return isPrefab() ? m_objects[0].lock()->getPrefabId() : std::string();
        }

    protected:
        //! Utils to collect shared components
        void collectSharedComponents();

    protected:
        //! List of objects
        std::vector<std::weak_ptr<SceneObject>> m_objects;
    };
} // namespace ige::scene
