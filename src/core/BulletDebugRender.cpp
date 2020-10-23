#include "core/BulletDebugRender.h"
#include "core/ShapeDrawer.h"

#include <utils/PhysicHelper.h>
using namespace ige::scene;

namespace ige::creator
{
    void BulletDebugRender::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        ShapeDrawer::drawLine(PhysicHelper::from_btVector3(from), PhysicHelper::from_btVector3(to), PhysicHelper::from_btVector3(color));
    }
} // namespace ige::creator