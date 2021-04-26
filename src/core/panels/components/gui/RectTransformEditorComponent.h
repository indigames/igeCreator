#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"


USING_NS_IGE_SCENE
NS_IGE_BEGIN

class RectTransformEditorComponent : public EditorComponent
{
public:
	RectTransformEditorComponent();
	~RectTransformEditorComponent();

	virtual void setTargetObject(const std::shared_ptr<SceneObject>& obj) override;
	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawRectTransform();

	void drawAnchor();
	void drawRect();
	void drawAnchorMinMax();
	void drawPivot();

	void onTransformChanged(SceneObject& sceneObject);

protected:
	std::shared_ptr<Group> m_pivotGroup = nullptr;
	std::shared_ptr<Group> m_anchor_transform_ParentGroup = nullptr;
	std::shared_ptr<Group> m_anchorGroup = nullptr;
	std::shared_ptr<Group> m_rectGroup = nullptr;
	std::shared_ptr<Group> m_anchorMinMaxGroup = nullptr;

	int m_dirtyFlagSupport = 0;
	bool m_bIsLockTransformUpdate = false;
	uint64_t m_listenerId = -1;
};

NS_IGE_END