#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"


USING_NS_IGE_SCENE
NS_IGE_BEGIN

class RectTransformEditorComponent : public EditorComponent
{
public:
	RectTransformEditorComponent();
	virtual ~RectTransformEditorComponent();
	virtual bool setComponent(std::shared_ptr<Component> component) override;

protected:
	virtual void onInspectorUpdate() override;
	void onTransformChanged(SceneObject& sceneObject);
	void drawRectTransform();

	void drawAnchor();
	void drawRect();
	void drawAnchorMinMax();
	void drawPivot();

	//! Target object
	void onTargetAdded(SceneObject* object);
	void onTargetRemoved(SceneObject* object);
	void onTargetCleared();
	void updateTarget();

protected:
	std::shared_ptr<Group> m_pivotGroup = nullptr;
	std::shared_ptr<Group> m_anchor_transform_ParentGroup = nullptr;
	std::shared_ptr<Group> m_anchorGroup = nullptr;
	std::shared_ptr<Group> m_rectGroup = nullptr;
	std::shared_ptr<Group> m_anchorMinMaxGroup = nullptr;

	int m_dirtyFlagSupport = 0;
	bool m_bIsLockTransformUpdate = false;
	uint64_t m_listenerId = -1;

	//! Targeted events
	uint64_t m_targetAddedEventId;
	uint64_t m_targetRemovedEventId;
	uint64_t m_targetClearedEventId;
	SceneObject* m_lastTarget = nullptr;
};

NS_IGE_END