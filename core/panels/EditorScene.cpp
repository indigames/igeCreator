#include <imgui.h>

#include "core/panels/EditorScene.h"
#include "core/widgets/Image.h"
#include "core/Editor.h"

using namespace pyxie;

namespace ige::creator
{
    EditorScene::EditorScene(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_editorShowcase = nullptr;
        m_editorEnvironment = nullptr;
        m_editorCamera = nullptr;
        m_fbo = nullptr;
    }
    
    EditorScene::~EditorScene()
    {
        clear();
    }

    void EditorScene::initialize()
    {
        clear();

        m_editorCamera = ResourceCreator::Instance().NewCamera("Editor_Camera", nullptr);
        m_editorCamera->LockonTarget(false);
        m_editorCamera->SetCameraPosition({0.f, 0.f, 0.f});

        m_editorEnvironment = ResourceCreator::Instance().NewEnvironmentSet("Editor_Environment", nullptr);
        m_editorEnvironment->SetAmbientGroundColor({0.5f, 0.5f, 0.5f});
        m_editorEnvironment->SetDirectionalLampColor(0, {0.5f, 0.5f, 0.5f});

        m_editorShowcase = ResourceCreator::Instance().NewShowcase();
        m_editorShowcase->Add(m_editorEnvironment);
        m_editorShowcase->Add(createGrid(Vec2(512.f, 512.f), 0));
    }

    EditableFigure* EditorScene::createGrid(const pyxie::Vec2& size, uint32_t textureId)
    {
        auto gen = pyxieResourceCreator::Instance().NewShaderDescriptor();
        gen->SetColorTexture(true);
        gen->SetVertexColor(true);

        auto figure = pyxieResourceCreator::Instance().NewEditableFigure("Editor_Grid");
        figure->AddMaterial("mate01", *gen);
        int index = figure->GetMaterialIndex(GenerateNameHash("mate01"));
        float buff[4] = { 1.0, 1.0, 1.0, 1.0 };
        figure->SetMaterialParam(index, "DiffuseColor", buff, ParamTypeFloat4);

        auto w = size.X() / 2.f;
        auto h = size.Y() / 2.f;
        auto points = { pyxie::Vec3(-w,h,0.f), pyxie::Vec3(w,h,0.f), pyxie::Vec3(-w,-h,0.f), pyxie::Vec3(w,-h,0.f)};
        auto uvs = {0.f, size.Y(), size.X(), size.Y(), 0.f, 0.f, size.X(), 0.f};
        auto tris = {0, 2, 1, 1, 2, 3, 1, 2, 0, 3, 2, 1};

        figure->AddMesh("mesh", "mate01");
        figure->SetMeshVertexValues(index, &points, points.size(), ATTRIBUTE_ID_POSITION, 0);
        figure->SetMeshVertexValues(index, &uvs, uvs.size(), ATTRIBUTE_ID_UV0, 0);
        figure->SetMeshIndices(index, 0, (const uint32_t*)&tris, tris.size(), 4);

        const ShaderParameterInfo* paramInfo = pyxieRenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
        uint32_t val[4] = { 1, 0, 0, 0 };
        figure->SetMaterialState(index, (ShaderParameterKey)paramInfo->key, val);

        Joint joint;
        figure->AddJoint(-1, joint, false, "joint");

        auto gridTexture = pyxieResourceCreator::Instance().NewTexture("grid");       
        gridTexture->WaitInitialize();

        Sampler sampler;
        sampler.tex = gridTexture;
        sampler.samplerSlot = 0;
        sampler.samplerState = { SamplerState::WRAP , SamplerState::WRAP , SamplerState::LINEAR , SamplerState::LINEAR , SamplerState::LINEAR };        
        Editor::getInstance()->getAssetSamplers().insert(std::pair<uint32_t, Sampler>(gridTexture->GetTextureHandle(), sampler));

        figure->SetMaterialParam(index, "ColorSampler", &sampler);
        figure->SetPosition({ 0.f, 0.f, 0.f });

        return figure;
    }

    void EditorScene::_drawImpl()
    {      
        auto size = ImVec2(getSize().x, getSize().y - 25.f); // title bar height
        if (!m_fbo && size.x > 0.f) {
            auto rtTexture = pyxieResourceCreator::Instance().NewTexture("Editor_RTTexture", nullptr, size.x, size.y, GL_RGBA);
            rtTexture->WaitInitialize();            

            Sampler sampler;
            sampler.tex = rtTexture;
            sampler.samplerSlot = 0;
            sampler.samplerState = { SamplerState::WRAP , SamplerState::WRAP , SamplerState::LINEAR , SamplerState::LINEAR , SamplerState::LINEAR };
            Editor::getInstance()->getAssetSamplers().insert(std::pair<uint32_t, Sampler>(rtTexture->GetTextureHandle(), sampler));

            m_fbo = ResourceCreator::Instance().NewRenderTarget(rtTexture, true, true);
            m_fbo->WaitInitialize();
                       
            m_image = createWidget<Image>(m_fbo->GetColorTexture()->GetTextureHandle(), size);
        }        

        auto* renderContext = RenderContext::InstancePtr();
        if (renderContext && m_fbo)
        {
            renderContext->BeginScene(m_fbo, Vec4(0.5f, 0.5f, 0.5f, 1.f), true, true);

            m_editorShowcase->Update(0.0f);
            m_editorCamera->Render();
            m_editorShowcase->Render();

            renderContext->EndScene();
        }

        Panel::_drawImpl();
    }

    void EditorScene::clear()
    {
        if(m_editorShowcase)
            m_editorShowcase->Clear();
        m_editorShowcase = nullptr;
        m_editorEnvironment = nullptr;
        m_editorCamera = nullptr;
    }
}
