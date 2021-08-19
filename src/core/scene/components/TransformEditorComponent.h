#include "core/Macros.h"

#include "core/scene/components/EditorComponent.h"
#include "components/Component.h"

USING_NS_IGE_SCENE
NS_IGE_BEGIN


class TransformEditorComponent : public EditorComponent
{
public:
    TransformEditorComponent();
    virtual ~TransformEditorComponent();

    virtual bool setComponent(std::shared_ptr<Component> component) override;

protected:
    //! Internal inspector update
    virtual void onInspectorUpdate() override;

    //! Draw local and world transform groups
    void drawLocalTransformComponent();
    void drawWorldTransformComponent();

    //! onTransformChanged
    void onTransformChanged(SceneObject& sceneObject);

    //! Target object
    void onTargetAdded(std::shared_ptr<SceneObject> object);
    void onTargetRemoved(std::shared_ptr<SceneObject> object);
    void onTargetCleared();
    void updateTarget();

protected:
    std::shared_ptr<Group> m_localTransformGroup = nullptr;
    std::shared_ptr<Group> m_worldTransformGroup = nullptr;

    //! Dirty flags
    int m_dirtyFlag = 0;

    //! Transform listener id
    uint64_t m_listenerId = (uint64_t)-1;

    //! Targeted events
    uint64_t m_targetAddedEventId;
    uint64_t m_targetRemovedEventId;
    uint64_t m_targetClearedEventId;
    SceneObject* m_lastTarget = nullptr;
};

NS_IGE_END