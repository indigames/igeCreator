#include "core/panels/InspectorEditor.h"

#include "core/widgets/Widgets.h"

#include "core/panels/components/CameraEditorComponent.h"
#include "core/panels/components/EnvironmentEditorComponent.h"
#include "core/panels/components/FigureEditorComponent.h"
#include "core/panels/components/SpriteEditorComponent.h"
#include "core/panels/components/ScriptEditorComponent.h"
#include "core/panels/components/TransformEditorComponent.h"

#include "core/panels/components/light/AmbientLightEditorComponent.h"
#include "core/panels/components/light/DirectionalLightEditorComponent.h"
#include "core/panels/components/light/PointLightEditorComponent.h"
#include "core/panels/components/light/SpotLightEditorComponent.h"

#include "core/panels/components/gui/CanvasEditorComponent.h"
#include "core/panels/components/gui/RectTransformEditorComponent.h"
#include "core/panels/components/gui/UIImageEditorComponent.h"
#include "core/panels/components/gui/UITextEditorComponent.h"
#include "core/panels/components/gui/UITextFieldEditorComponent.h"
#include "core/panels/components/gui/UIButtonEditorComponent.h"

#include "core/panels/components/physic/PhysicManagerEditorComponent.h"
#include "core/panels/components/physic/PhysicBoxEditorComponent.h"
#include "core/panels/components/physic/PhysicSphereEditorComponent.h"
#include "core/panels/components/physic/PhysicCapsuleEditorComponent.h"
#include "core/panels/components/physic/PhysicMeshEditorComponent.h"
#include "core/panels/components/physic/PhysicSoftBodyEditorComponent.h"

#include "core/panels/components/audio/AudioManagerEditorComponent.h"
#include "core/panels/components/audio/AudioSourceEditorComponent.h"
#include "core/panels/components/audio/AudioListenerEditorComponent.h"

#include "core/panels/components/particle/ParticleManagerEditorComponent.h"
#include "core/panels/components/particle/ParticleEditorComponent.h"

#include "core/panels/components/navigation/NavigableEditorComponent.h"
#include "core/panels/components/navigation/NavMeshEditorComponent.h"
#include "core/panels/components/navigation/NavAgentEditorComponent.h"
#include "core/panels/components/navigation/NavAgentManagerEditorComponent.h"
#include "core/panels/components/navigation/DynamicNavMeshEditorComponent.h"
#include "core/panels/components/navigation/NavAreaEditorComponent.h"
#include "core/panels/components/navigation/NavObstacleEditorComponent.h"
#include "core/panels/components/navigation/OffMeshLinkEditorComponent.h"


NS_IGE_BEGIN

InspectorEditor::InspectorEditor() {
	m_componentGroup = nullptr;
	m_targetObject = nullptr;
	m_deltaTime = 0;
}

InspectorEditor::~InspectorEditor() {
	clear();

	if (m_componentGroup)
	{
		m_componentGroup->removeAllWidgets();
		m_componentGroup->removeAllPlugins();
		m_componentGroup = nullptr;
	}
}

void InspectorEditor::clear() {
	std::map<uint64_t, std::shared_ptr<Group>> releasings;
	releasings.swap(m_groups);
	for (const auto& obj : releasings)
	{
		if (obj.second != nullptr) {
			obj.second->removeAllWidgets();
			obj.second->removeAllPlugins();
		}
	}

	m_components.clear();
}
	
void InspectorEditor::setParentGroup(std::shared_ptr<Group> componentGroup)
{
	if (m_componentGroup != componentGroup) {
		if (m_componentGroup) {
			m_componentGroup->removeAllWidgets();
			m_componentGroup->removeAllPlugins();
		}
		m_componentGroup = componentGroup;
	}
}

void InspectorEditor::setTargetObject(const std::shared_ptr<SceneObject>& obj)
{
	if (m_targetObject != obj) {
		m_targetObject = obj;

		for (auto& comp : m_components) {
			if(comp.second != nullptr)
				comp.second->setTargetObject(obj);
		}
	}
}

std::shared_ptr<EditorComponent> InspectorEditor::addComponent(int type, Component* comp, std::shared_ptr<Group> header) {
	if (comp == nullptr) return nullptr;

	ComponentType m_type = (ComponentType)type;

	std::shared_ptr<EditorComponent> view = nullptr;
	switch (m_type) {
	case ComponentType::Camera:
	{
		view = std::make_shared<CameraEditorComponent>();
	}
	break;
	case ComponentType::Environment:
	{
		view = std::make_shared<EnvironmentEditorComponent>();
	}
	break;
	case ComponentType::Figure:
	{
		view = std::make_shared<FigureEditorComponent>();
	}
	break;
	case ComponentType::Sprite:
	{
		view = std::make_shared<SpriteEditorComponent>();
	}
	break;
	case ComponentType::Script:
	{
		view = std::make_shared<ScriptEditorComponent>();
	}
	break;
	case ComponentType::AmbientLight:
	{
		view = std::make_shared<AmbientLightEditorComponent>();
	}
	break;
	case ComponentType::DirectionalLight:
	{
		view = std::make_shared<DirectionalLightEditorComponent>();
	}
	break;
	case ComponentType::PointLight:
	{
		view = std::make_shared<PointLightEditorComponent>();
	}
	break;
	case ComponentType::SpotLight:
	{
		view = std::make_shared<SpotLightEditorComponent>();
	}
	break;
	case ComponentType::Canvas:
	{
		view = std::make_shared<CanvasEditorComponent>();
	}
	break;
	case ComponentType::UIImage:
	{
		view = std::make_shared<UIImageEditorComponent>();
	}
	break;
	case ComponentType::UIText:
	{
		view = std::make_shared<UITextEditorComponent>();
	}
	break;
	case ComponentType::UITextField:
	{
		view = std::make_shared<UITextFieldEditorComponent>();
	}
	break;
	case ComponentType::UIButton:
	{
		view = std::make_shared<UIButtonEditorComponent>();
	}
	break;
	case ComponentType::PhysicManager:
	{
		view = std::make_shared<PhysicManagerEditorComponent>();
	}
	break;
	case ComponentType::PhysicBox:
	{
		view = std::make_shared<PhysicBoxEditorComponent>();
	}
	break;
	case ComponentType::PhysicSphere:
	{
		view = std::make_shared<PhysicSphereEditorComponent>();
	}
	break;
	case ComponentType::PhysicCapsule:
	{
		view = std::make_shared<PhysicCapsuleEditorComponent>();
	}
	break;
	case ComponentType::PhysicMesh:
	{
		view = std::make_shared<PhysicMeshEditorComponent>();
	}
	break;
	case ComponentType::PhysicSoftBody:
	{
		view = std::make_shared<PhysicSoftBodyEditorComponent>();
	}
	break;
	case ComponentType::AudioManager:
	{
		view = std::make_shared<AudioManagerEditorComponent>();
	}
	break;
	case ComponentType::AudioSource:
	{
		view = std::make_shared<AudioSourceEditorComponent>();
	}
	break;
	case ComponentType::AudioListener:
	{
		view = std::make_shared<AudioListenerEditorComponent>();
	}
	break;
	case ComponentType::ParticleManager:
	{
		view = std::make_shared<ParticleManagerEditorComponent>();
	}
	break;
	case ComponentType::Particle:
	{
		view = std::make_shared<ParticleEditorComponent>();
	}
	break;
	case ComponentType::Navigable:
	{
		view = std::make_shared<NavigableEditorComponent>();
	}
	break;
	case ComponentType::NavMesh:
	{
		view = std::make_shared<NavMeshEditorComponent>();
	}
	break;
	case ComponentType::NavAgent:
	{
		view = std::make_shared<NavAgentEditorComponent>();
	}
	break;
	case ComponentType::NavAgentManager:
	{
		view = std::make_shared<NavAgentManagerEditorComponent>();
	}
	break;
	case ComponentType::DynamicNavMesh:
	{
		view = std::make_shared<DynamicNavMeshEditorComponent>();
	}
	break;
	case ComponentType::NavArea:
	{
		view = std::make_shared<NavAgentManagerEditorComponent>();
	}
	break;
	case ComponentType::NavObstacle:
	{
		view = std::make_shared<NavObstacleEditorComponent>();
	}
	break;
	case ComponentType::OffMeshLink:
	{
		view = std::make_shared<OffMeshLinkEditorComponent>();
	}
	break;
	case ComponentType::Transform:
	{
		view = std::make_shared<TransformEditorComponent>();
	}
	break;
	case ComponentType::RectTransform:
	{
		view = std::make_shared<RectTransformEditorComponent>();
	}
	break;
	default:
		return nullptr;
	}

	if (view == nullptr) return nullptr;

	view->setComponent(comp);
	view->setTargetObject(m_targetObject);

	m_groups[comp->getInstanceId()] = header;
	m_components[comp->getInstanceId()] = view;
	view->draw(header);

	return view;
}

void InspectorEditor::removeComponent(uint64_t componentInstanceId) {
	m_groups[componentInstanceId] = nullptr;
	m_components[componentInstanceId] = nullptr;
	m_components.erase(componentInstanceId);
}

void InspectorEditor::update(float dt) {
	m_deltaTime += dt;
	if (m_deltaTime < INSPECTOR_EDITOR_DELTA_LIMIT) return;
	m_deltaTime = 0;
	
	try {
		for (auto const& component : m_components) {
			if (component.second != nullptr && component.second->isDirty()) {
				component.second->redraw();
			}
		}
	}
	catch(...)
	{
		pyxie_printf("ERROR !!!");
	}

	/*for (auto& watch : m_watcher) {
		for (auto& _obj : watch.second) {
			auto vec = (Vec3*)_obj.getAddress();
			auto old = std::any_cast<Vec3>(_obj.getValue());
			if (*vec != old) {
				_obj.setValue(*vec);
				makeDirty(watch.first);
			}
			
		}
	}*/
}

void InspectorEditor::makeDirty(Component* comp) {
	std::shared_ptr<EditorComponent> view = nullptr;
	for (auto obj : m_components) {
		if (obj.second != nullptr && obj.second->getComponent() == comp) {
			view = obj.second;
			break;
		}
	}
	if (view == nullptr) return;
	view->setDirty(true);
}

void InspectorEditor::makeDirty(uint64_t componentInstanceId) {
	std::shared_ptr<EditorComponent> view = nullptr;
	for (auto obj : m_components) {
		if (obj.second != nullptr && obj.second->getComponent()->getInstanceId() == componentInstanceId) {
			view = obj.second;
			break;
		}
	}
	if (view == nullptr) return;
	view->setDirty(true);
}

void InspectorEditor::addWatcherValue(uint64_t componentInstanceId, std::type_index _typeId, void* address, std::any value)
{
	if (m_watcher[componentInstanceId].size() == 0)
		m_watcher[componentInstanceId].reserve(10);
	m_watcher[componentInstanceId].push_back(WatcherPair(_typeId, address, value));
}

NS_IGE_END