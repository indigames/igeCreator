#pragma once

#include <memory>
#include <vector>
#include <string>

#include <components/Component.h>

namespace ige::scene
{
    /**
    * CompoundComponent represent group of component
    */
    class CompoundComponent : public Component
    {
    public:
        //! Constructor
        CompoundComponent(SceneObject &owner);

        //! Destructor
        virtual ~CompoundComponent();

        // Inherited via Component
        virtual std::string getName() const override;

        //! Returns the type of the component
        virtual Type getType() const override;

        std::vector<std::shared_ptr<Component>>& getComponents() { return m_components; }

        //! Add/remove/clear components
        void add(const std::shared_ptr<Component>& comp);
        bool remove(const std::shared_ptr<Component>& comp);
        bool remove(uint64_t compId);
        void clear();

        //! Check if the targets is empty
        inline bool empty() const { return m_components.empty(); }

        //! Get the number of targets
        inline size_t size() const { return m_components.size(); }

        //! Update json value
        virtual void setProperty(const std::string& key, const json& val) override;

        //! Serialize
        virtual void to_json(json& j) const override;

        //! Set dirty to reload components
        void setDirty() { collectSharedElements(); }

        //! Enable
        virtual void onEnable() override;

        //! Disable
        virtual void onDisable() override;

        //! Update functions
        virtual void onUpdate(float dt) override;
        virtual void onFixedUpdate(float dt) override;
        virtual void onLateUpdate(float dt) override;

        //! Render
        virtual void onRender() override;

        //! Destroyed
        virtual void onDestroy() override;

        //! Click
        virtual void onClick() override;

        //! Suspend
        virtual void onSuspend() override;

        //! Resume
        virtual void onResume() override;

    protected:
        //! Deserialize
        virtual void from_json(const json &j) override {}

        //! Utils to collect shared elements
        void collectSharedElements();

    protected:
        //! List of internal components
        std::vector<std::shared_ptr<Component>> m_components;

        //! json representation
        json m_json = {};

    };
} // namespace ige::scene
