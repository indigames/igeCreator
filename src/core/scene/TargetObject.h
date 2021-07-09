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

        // Get parent
        virtual SceneObject *getParent() const override;

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
        void add(SceneObject* object);
        void remove(SceneObject* object);
        void clear();

        //! Check if the targets is empty
        inline bool empty() const { return m_objects.empty(); }

        //! Get the number of targets
        inline size_t size() const { return m_objects.size(); }
        
        //! Get first target
        SceneObject* getFirstTarget();

        //! Get all targets
        std::vector<SceneObject*>& getAllTargets();

    protected:
        //! Utils to collect shared components
        void collectSharedComponents();

    protected:
        //! List of objects
        std::vector<SceneObject*> m_objects;
    };
} // namespace ige::scene
