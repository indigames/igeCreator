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

	void drawRectTransformAnchor();
	void drawAnchor();
	void drawTransform();

	void onTransformChanged(SceneObject& sceneObject);

protected:
	std::shared_ptr<Group> m_rectTransformGroup = nullptr;
	std::shared_ptr<Group> m_rectTransformAnchorGroup = nullptr;
	std::shared_ptr<Group> m_anchorGroup = nullptr;
	std::shared_ptr<Group> m_anchorTransformGroup = nullptr;

	int m_dirtyFlagSupport = 0;
};

NS_IGE_END