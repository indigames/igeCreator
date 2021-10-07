#include "core/panels/InspectorEditor.h"

#include "core/widgets/Widgets.h"

#include "core/scene/components/CameraEditorComponent.h"
#include "core/scene/components/EnvironmentEditorComponent.h"
#include "core/scene/components/FigureEditorComponent.h"
#include "core/scene/components/SpriteEditorComponent.h"
#include "core/scene/components/TextEditorComponent.h"
#include "core/scene/components/TextBitmapEditorComponent.h"
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
#include "core/scene/components/gui/UITextBitmapEditorComponent.h"
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
	if (comp == nullptr)
		return nullptr;

	auto m_type = (Component::Type)type;

	std::shared_ptr<EditorComponent> view = nullptr;
	switch (m_type) {
	case Component::Type::Camera:
		view = std::make_shared<CameraEditorComponent>();
	break;
	case Component::Type::Environment:
		view = std::make_shared<EnvironmentEditorComponent>();
	break;
	case Component::Type::Figure:
		view = std::make_shared<FigureEditorComponent>();
	break;	
	case Component::Type::BoneTransform:
		view = std::make_shared<BoneTransformEditorComponent>();
	break;
	case Component::Type::Sprite:
		view = std::make_shared<SpriteEditorComponent>();
	break;
	case Component::Type::Text:
		view = std::make_shared<TextEditorComponent>();
	break;
	case Component::Type::TextBitmap:
		view = std::make_shared<TextBitmapEditorComponent>();
		break;
	case Component::Type::Script:
		view = std::make_shared<ScriptEditorComponent>();
	break;
	case Component::Type::AmbientLight:
		view = std::make_shared<AmbientLightEditorComponent>();
	break;
	case Component::Type::DirectionalLight:
		view = std::make_shared<DirectionalLightEditorComponent>();
	break;
	case Component::Type::PointLight:
		view = std::make_shared<PointLightEditorComponent>();
	break;
	case Component::Type::SpotLight:
		view = std::make_shared<SpotLightEditorComponent>();
	break;
	case Component::Type::Canvas:
		view = std::make_shared<CanvasEditorComponent>();
	break;
	case Component::Type::UIImage:
		view = std::make_shared<UIImageEditorComponent>();
	break;
	case Component::Type::UIText:
		view = std::make_shared<UITextEditorComponent>();
	break;
	case Component::Type::UITextBitmap:
		view = std::make_shared<UITextBitmapEditorComponent>();
	break;
	case Component::Type::UITextField:
		view = std::make_shared<UITextFieldEditorComponent>();
	break;
	case Component::Type::UIButton:
		view = std::make_shared<UIButtonEditorComponent>();
	break;
	case Component::Type::UISlider:
		view = std::make_shared<UISliderEditorComponent>();
	break;
	case Component::Type::UIScrollView:
		view = std::make_shared<UIScrollViewEditorComponent>();
	break;
	case Component::Type::UIMask:
		view = std::make_shared<UIMaskEditorComponent>();
	break;
	case Component::Type::UIScrollBar:
		view = std::make_shared<UIScrollBarEditorComponent>();
	break;
	case Component::Type::PhysicManager:
		view = std::make_shared<PhysicManagerEditorComponent>();
	break;
	case Component::Type::PhysicBox:
		view = std::make_shared<PhysicBoxEditorComponent>();
	break;
	case Component::Type::PhysicSphere:
		view = std::make_shared<PhysicSphereEditorComponent>();
	break;
	case Component::Type::PhysicCapsule:
		view = std::make_shared<PhysicCapsuleEditorComponent>();	
	break;
	case Component::Type::PhysicMesh:
		view = std::make_shared<PhysicMeshEditorComponent>();
	break;
	case Component::Type::PhysicSoftBody:
		view = std::make_shared<PhysicSoftBodyEditorComponent>();
	break;
	case Component::Type::AudioManager:
		view = std::make_shared<AudioManagerEditorComponent>();
	break;
	case Component::Type::AudioSource:
		view = std::make_shared<AudioSourceEditorComponent>();
	break;
	case Component::Type::AudioListener:
		view = std::make_shared<AudioListenerEditorComponent>();
	break;
	case Component::Type::ParticleManager:
		view = std::make_shared<ParticleManagerEditorComponent>();
	break;
	case Component::Type::Particle:
		view = std::make_shared<ParticleEditorComponent>();
	break;
	case Component::Type::Navigable:
		view = std::make_shared<NavigableEditorComponent>();
	break;
	case Component::Type::NavMesh:
		view = std::make_shared<NavMeshEditorComponent>();
	break;
	case Component::Type::NavAgent:
		view = std::make_shared<NavAgentEditorComponent>();
	break;
	case Component::Type::NavAgentManager:
		view = std::make_shared<NavAgentManagerEditorComponent>();
	break;
	case Component::Type::DynamicNavMesh:
		view = std::make_shared<DynamicNavMeshEditorComponent>();
	break;
	case Component::Type::NavArea:
		view = std::make_shared<NavAgentManagerEditorComponent>();
	break;
	case Component::Type::NavObstacle:
		view = std::make_shared<NavObstacleEditorComponent>();
	break;
	case Component::Type::OffMeshLink:
		view = std::make_shared<OffMeshLinkEditorComponent>();
	break;
	case Component::Type::Transform:
		view = std::make_shared<TransformEditorComponent>();
	break;
	case Component::Type::RectTransform:
		view = std::make_shared<RectTransformEditorComponent>();
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