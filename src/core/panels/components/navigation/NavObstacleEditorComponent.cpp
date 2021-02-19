#include "core/panels/components/navigation/NavObstacleEditorComponent.h"

#include <core/layout/Group.h>

#include "components/navigation/NavObstacle.h"
#include "core/widgets/Widgets.h"
#include "core/layout/Columns.h"

NS_IGE_BEGIN

NavObstacleEditorComponent::NavObstacleEditorComponent() {
    m_navObstacleGroup = nullptr;
}

NavObstacleEditorComponent::~NavObstacleEditorComponent()
{
    if (m_navObstacleGroup) {
        m_navObstacleGroup->removeAllWidgets();
        m_navObstacleGroup->removeAllPlugins();
    }
    m_navObstacleGroup = nullptr;
}

bool NavObstacleEditorComponent::isSafe(Component* comp)
{
    return dynamic_cast<NavObstacle*>(comp);
}

void NavObstacleEditorComponent::redraw()
{
    if (m_group == nullptr)
        return;

    if (m_navObstacleGroup == nullptr) {
        m_navObstacleGroup = m_group->createWidget<Group>("NavObstacle", false);
    }
    drawNavObstacle();

    EditorComponent::redraw();
}

void NavObstacleEditorComponent::onInspectorUpdate()
{
    if (m_group == nullptr)
        return;
    m_group->removeAllWidgets();

    m_navObstacleGroup = m_group->createWidget<Group>("NavObstacle", false);

    drawNavObstacle();
}

void NavObstacleEditorComponent::drawNavObstacle()
{
    if (m_navObstacleGroup == nullptr)
        return;
    m_navObstacleGroup->removeAllWidgets();

    auto obstacle = m_targetObject->getComponent<NavObstacle>();
    if (obstacle == nullptr)
        return;

    auto column = m_navObstacleGroup->createWidget<Columns<3>>();
    column->createWidget<CheckBox>("Enable", obstacle->isEnabled())->getOnDataChangedEvent().addListener([this](bool val) {
        auto obstacle = m_targetObject->getComponent<NavObstacle>();
        obstacle->setEnabled(val);
        });

    std::array radius = { obstacle->getRadius() };
    m_navObstacleGroup->createWidget<Drag<float>>("Radius", ImGuiDataType_Float, radius, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto obstacle = m_targetObject->getComponent<NavObstacle>();
        obstacle->setRadius(val[0]);
        });

    std::array height = { obstacle->getHeight() };
    m_navObstacleGroup->createWidget<Drag<float>>("Height", ImGuiDataType_Float, height, 0.001f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
        auto obstacle = m_targetObject->getComponent<NavObstacle>();
        obstacle->setHeight(val[0]);
        });
}
NS_IGE_END