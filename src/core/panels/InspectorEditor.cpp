#include "core/panels/InspectorEditor.h"

#include "core/widgets/Widgets.h"

#include "core/scene/components/CameraEditorComponent.h"
#include "core/scene/components/EnvironmentEditorComponent.h"
#include "core/scene/components/FigureEditorComponent.h"
#include "core/scene/components/SpriteEditorComponent.h"
#include "core/scene/components/ScriptEditorComponent.h"
#include "core/scene/components/TransformEditorComponent.h"
#include "core/scene/components/BoneTransformEditorComponent.h"

#include "core/scene/components/light/AmbientLightEditorComponent.h"
#include "core/scene/components/light/DirectionalLightEditorComponent.h"
#include "core/scene/components/light/PointLightEditorComponent.h"
#include "core/scene/components/light/SpotLightEditorComponent.h"

#include "core/scene/components/gui/CanvasEditorComponent.h"
#include "core/scene/components/gui/RectTransformEditorComponent.h"
#include "core/scene/components/gui/UIImageEditorComponent.h"
#include "core/scene/components/gui/UITextEditorComponent.h"
#include "core/scene/components/gui/UITextFieldEditorComponent.h"
#include "core/scene/components/gui/UIButtonEditorComponent.h"
#include "core/scene/components/gui/UISliderEditorComponent.h"
#include "core/scene/components/gui/UIScrollViewEditorComponent.h"
#include "core/scene/components/gui/UIScrollBarEditorComponent.h"
#include "core/scene/components/gui/UIMaskEditorComponent.h"

#include "core/scene/components/physic/PhysicManagerEditorComponent.h"
#include "core/scene/components/physic/PhysicBoxEditorComponent.h"
#include "core/scene/components/physic/PhysicSphereEditorComponent.h"
#include "core/scene/components/physic/PhysicCapsuleEditorComponent.h"
#include "core/scene/components/physic/PhysicMeshEditorComponent.h"
#include "core/scene/components/physic/PhysicSoftBodyEditorComponent.h"

#include "core/scene/components/audio/AudioManagerEditorComponent.h"
#include "core/scene/components/audio/AudioSourceEditorComponent.h"
#include "core/scene/components/audio/AudioListenerEditorComponent.h"

#include "core/scene/components/particle/ParticleManagerEditorComponent.h"
#include "core/scene/components/particle/ParticleEditorComponent.h"

#include "core/scene/components/navigation/NavigableEditorComponent.h"
#include "core/scene/components/navigation/NavMeshEditorComponent.h"
#include "core/scene/components/navigation/NavAgentEditorComponent.h"
#include "core/scene/components/navigation/NavAgentManagerEditorComponent.h"
#include "core/scene/components/navigation/DynamicNavMeshEditorComponent.h"
#include "core/scene/components/navigation/NavAreaEditorComponent.h"
#include "core/scene/components/navigation/NavObstacleEditorComponent.h"
#include "core/scene/components/navigation/OffMeshLinkEditorComponent.h"


NS_IGE_BEGIN

InspectorEditor::InspectorEditor() {
	m_deltaTime = 0;
}

InspectorEditor::~InspectorEditor() {
	clear();
}

void InspectorEditor::clear() {
	for (auto& [key, value] : m_groups) {
		value = nullptr;
	}
	m_groups.clear();

	for (auto& [key, value] : m_components) {
		value = nullptr;
	}
	m_components.clear();
}

std::shared_ptr<EditorComponent> InspectorEditor::addComponent(int type, std::shared_ptr<Component> comp, std::shared_ptr<Group> header) {
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
	case ComponentType::BoneTransform:
	{
		view = std::make_shared<BoneTransformEditorComponent>();
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
	case ComponentType::UISlider:
	{
		view = std::make_shared<UISliderEditorComponent>();
	}
	break;
	case ComponentType::UIScrollView:
	{
		view = std::make_shared<UIScrollViewEditorComponent>();
	}
	break;
	case ComponentType::UIMask:
	{
		view = std::make_shared<UIMaskEditorComponent>();
	}
	break;
	case ComponentType::UIScrollBar:
	{
		view = std::make_shared<UIScrollBarEditorComponent>();
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
	
	for(const auto& component: m_components) {
		if (component.second != nullptr && component.second->isDirty()) {
			component.second->redraw();
		}
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