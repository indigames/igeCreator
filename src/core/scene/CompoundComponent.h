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

        //! Get containee name
        std::string getContainName();

        std::vector<std::shared_ptr<Component>>& getComponents() { return m_components; }

        //! Enable/Disable
        virtual bool isEnabled() const override;
        virtual void setEnabled(bool enable = true) override;

        //! Add/remove/clear components
        void add(const std::shared_ptr<Component>& comp);
        bool remove(const std::shared_ptr<Component>& comp);
        bool remove(uint64_t compId);
        void clear();

        //! Check if the targets is empty
        inline bool empty() const { return m_components.empty(); }

        //! Get the number of targets
        inline size_t size() const { return m_components.size(); }

    protected:
        //! Serialize
        virtual void to_json(json &j) const override {}

        //! Deserialize
        virtual void from_json(const json &j) override {}

    protected:
        //! List of internal components
        std::vector<std::shared_ptr<Component>> m_components;

    };
} // namespace ige::scene
