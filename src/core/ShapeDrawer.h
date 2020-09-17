#pragma once

#include <utils/PyxieHeaders.h>

namespace ige::creator
{
    //! ShapeDrawer
    class ShapeDrawer
    {
    public:
        static void initialize();

        static void setViewProjectionMatrix(const Mat4 &viewProjection);
        static void drawLine(const Vec3 &start, const Vec3 &end, const Vec3 &color = { 0.f, 1.f, 0.f }, float width = 1.f);
    };
} // namespace ige::creator