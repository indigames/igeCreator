#include "core/Macros.h"

#include <map>
#include <vector>
#include <typeindex>
#include <any>

#include "scene/SceneObject.h"
#include <core/layout/Group.h>
#include "components/Component.h"
#include "core/scene/components/EditorComponent.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

struct WatcherPair 
{
	WatcherPair(std::type_index type, void* address, std::any value) : m_type_id(type), m_address(address) , m_value(value) { }
	virtual ~WatcherPair() {}

	std::type_index getTypeId() { return m_type_id; }
	void* getAddress() { return m_address; }

	std::any getValue() { return m_value; }
	void setValue(std::any value) { m_value = value; }

protected:
	std::type_index m_type_id;
	void* m_address;
	std::any m_value;
};

class InspectorEditor
{
public:
	InspectorEditor();
	virtual ~InspectorEditor();

	void clear();
	void update(float dt);

	std::shared_ptr<EditorComponent> addComponent(int type, std::shared_ptr<Component> component, std::shared_ptr<Group> header);
	void removeComponent(uint64_t componentInstanceId);

	void makeDirty(Component::Type componentType);
	void addWatcherValue(uint64_t componentInstanceId, std::type_index _typeId, void* address, std::any value);

protected:
	std::map<uint64_t, std::shared_ptr<EditorComponent>> m_components = {};
	std::map<uint64_t, std::shared_ptr<Group>> m_groups = {};
	std::map<uint64_t, std::vector<WatcherPair>> m_watcher = {};
	float m_deltaTime;
};


NS_IGE_END