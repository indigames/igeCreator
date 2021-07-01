#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class CameraEditorComponent : public EditorComponent
{
public:
	CameraEditorComponent();
	virtual ~CameraEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	virtual void drawCameraComponent();

	void onTransformChanged();
protected:
	
	std::shared_ptr<Group> m_cameraCompGroup = nullptr;
	std::shared_ptr<Group> m_cameraLockTargetGroup = nullptr;
};

NS_IGE_END