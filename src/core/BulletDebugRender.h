#pragma once

#include <LinearMath/btIDebugDraw.h>

#include <utils/PyxieHeaders.h>
using namespace pyxie;

namespace ige::creator
{
    //! BulletDebugRender
    class BulletDebugRender: public btIDebugDraw
    {
    public:
        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
        virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {}
        virtual void reportErrorWarning(const char* warningString) override {}

        virtual void draw3dText(const btVector3& location, const char* textString) override {}

        virtual void setDebugMode(int debugMode) override {}

        virtual int getDebugMode() const override { return 1; }
    };
} // namespace ige::creator