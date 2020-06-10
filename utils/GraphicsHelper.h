#pragma once

#include "utils/Singleton.h"
#include "utils/PyxieHeaders.h"

using namespace pyxie;

namespace ige::creator
{
    class GraphicsHelper : public Singleton<GraphicsHelper>
    {
    public:
        EditableFigure* createSprite(const Vec2& size = {128, 128}, const std::string& texture = "", const Vec2& uv_top_left = {0.f, 0.f}, const Vec2& uv_bottom_right = {1.f, 1.f}, Vec3* normal = nullptr, uint32_t pivot = 4, ShaderDescriptor* shader = nullptr);
        EditableFigure* createMesh(const std::vector<float>& points, const std::vector<uint32_t>& trianglesIndices, const std::string& texture = "", std::vector<float>* uvs = nullptr, ShaderDescriptor* shader = nullptr, Vec3* normal = nullptr);
        EditableFigure* createText(const std::string& words, const std::string& fontPath, const Vec2& fontSize, const Vec4& color = {1.f, 1.f, 1.f, 1.f}, uint32_t pivot = 4, float scale = 1.0f);

    protected:
        const std::vector<float> _uvs = {0.f, 1.f, 1.f, 1.f, 0.f, 0.f, 1.f, 0.f};
        const std::vector<uint32_t> _tris = {0, 2, 1, 1, 2, 3};
        const std::vector<int32_t> _pivotoffset = {1,-1, 0,-1, -1,-1, 1,0, 0,0, -1,0, 1,1, 0,1, -1,1};
    };
}
