#ifndef __NAV_OBSTACLE_EDITOR_COMPONENT_H_
#define __NAV_OBSTACLE_EDITOR_COMPONENT_H_

#include "core/Macros.h"

#include "core/panels/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN

class NavObstacleEditorComponent : public EditorComponent
{
public:
	NavObstacleEditorComponent();
	virtual ~NavObstacleEditorComponent();

	virtual void redraw() override;
protected:
	
	virtual void onInspectorUpdate() override;

	void drawNavObstacle();
protected:
	std::shared_ptr<Group> m_navObstacleGroup = nullptr;

	
};

NS_IGE_END

#endif //__NAV_OBSTACLE_EDITOR_COMPONENT_H_