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

        //! Return json value
        const json& getJson() const { return m_json; }

        //! Update whole json
        void setJson(const json& val);

        //! Get json value by key
        template <typename T>
        T getJson(const std::string& key, const T& defaultVal);

        //! Update json value
        void setJson(const std::string& key, const json& val);

    protected:
        //! Serialize
        virtual void to_json(json &j) const override {}

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

    //! Get json value by key
    template <typename T>
    T CompoundComponent::getJson(const std::string& key, const T& defaultVal)
    {
        return (m_json.contains(key) && !m_json.at(key).is_null()) ? m_json.value(key, defaultVal) : defaultVal;
    }
} // namespace ige::scene
