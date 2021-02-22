#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN


class TransformEditorComponent : public EditorComponent
{
public:
	TransformEditorComponent();
	~TransformEditorComponent();

	virtual void setTargetObject(const std::shared_ptr<SceneObject>& obj) override;
	virtual void redraw() override;
protected:
	virtual bool isSafe(Component* comp) override;
	virtual void onInspectorUpdate() override;

	void drawLocalTransformComponent();
	void drawWorldTransformComponent();

	//! onTransformChanged
	void onTransformChanged(SceneObject& sceneObject);

protected:
	std::shared_ptr<Group> m_localTransformGroup = nullptr;
	std::shared_ptr<Group> m_worldTransformGroup = nullptr;
	int m_dirtyFlag = 0;
	
};

NS_IGE_END