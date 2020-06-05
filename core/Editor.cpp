#include <imgui.h>

#include <pyxieResourceCreator.h>
#include <pyxieResourceManager.h>
#include <pyxieSystemInfo.h>
#include <pyxieShaderDescriptor.h>

#include "core/Editor.h"
#include "core/Canvas.h"
#include "core/panels/MenuBar.h"

using namespace pyxie;

namespace ige::creator
{
    Editor::Editor()
    {}
    
    Editor::~Editor()
    {
        m_canvas = nullptr;
        m_app.reset();
        
        m_editableFigure = nullptr;
        m_showcase = nullptr;
        m_fontTexture = nullptr;
        m_camera = nullptr;
    }
    
    void Editor::initialize()
    {
        initImGUI();

        m_canvas = std::make_shared<Canvas>();
        m_canvas->createPanel<MenuBar>("Menu");
        m_canvas->setDockable(true);
    }

    void Editor::handleEvent(const void* event)
    {
        Editor::getInstance()->handleEventImGUI((const SDL_Event*)event);
    }

    void Editor::update(float dt)
    {
        updateImGUI();
    }
            
    void Editor::render()
    {
        m_canvas->draw();
        renderImGUI();

        pyxieRenderContext::InstancePtr()->BeginScene(nullptr, Vec4(0.2f, 0.6f, 0.8f, 0.0f), true, false);
        m_showcase->Update(0.0f);
        m_camera->Render();
        m_showcase->Render();
        pyxieRenderContext::InstancePtr()->EndScene();
    } 

    void Editor::createDeviceObjects()
    {
        m_camera = pyxieResourceCreator::Instance().NewCamera("camera_2d", nullptr);
        m_camera->SetOrthographicProjection(true);
        m_camera->SetPosition(Vec3(0, 0, 1));
        m_camera->SetScreenScale(Vec2(1, -1));
        m_camera->SetScreenOffset(Vec2(-pyxieSystemInfo::Instance().GetGameW(), pyxieSystemInfo::Instance().GetGameH()));
        m_camera->SetTarget(Vec3(0, 0, 0));

        auto shaderDescriptor = pyxieResourceCreator::Instance().NewShaderDescriptor();
        shaderDescriptor->SetColorTexture(true);
        shaderDescriptor->SetVertexColor(true);

        m_editableFigure = pyxieResourceCreator::Instance().NewEditableFigure("font");
        m_editableFigure->AddMaterial("mate01", *shaderDescriptor);        
        int index = m_editableFigure->GetMaterialIndex(GenerateNameHash("mate01"));
        float buff[4] = { 1.0, 1.0, 1.0, 1.0 };
        m_editableFigure->SetMaterialParam(index, "DiffuseColor", buff, ParamTypeFloat4);

        auto paramInfo = pyxieRenderContext::Instance().GetShaderParameterInfoByName("blend_enable");
        uint32_t val[4] = { 1, 0, 0, 0 };
        m_editableFigure->SetMaterialState(index, (ShaderParameterKey)paramInfo->key, val);

        Joint joint;
        m_editableFigure->AddJoint(-1, joint, false, "joint");

        m_showcase = pyxieResourceCreator::Instance().NewShowcase();
        m_showcase->Add(m_editableFigure);
    }

    void Editor::refreshFontTexture()
    {
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* out_pixels;
        int out_width;
        int out_height;
        io.Fonts->GetTexDataAsRGBA32(&out_pixels, &out_width, &out_height);
        m_fontTexture = pyxieResourceCreator::Instance().NewTexture("fonttex", reinterpret_cast<const char*>(out_pixels), out_width, out_height, GL_RGBA);

        int index = m_editableFigure->GetMaterialIndex(GenerateNameHash("mate01"));
        Sampler sampler;
        sampler.tex = m_fontTexture;
        sampler.samplerSlot = 0;
        sampler.samplerState = { SamplerState::WRAP , SamplerState::WRAP , SamplerState::LINEAR , SamplerState::LINEAR , SamplerState::LINEAR };
        m_editableFigure->SetMaterialParam(index, "ColorSampler", &sampler);

        m_assetSamplers.insert(std::pair<uint32_t, Sampler>(m_fontTexture->GetTextureHandle(), sampler));
        io.Fonts->SetTexID((void*)m_fontTexture->GetTextureHandle());
    }

    void Editor::initImGUI()
    {
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { pyxieSystemInfo::Instance().GetGameW(), pyxieSystemInfo::Instance().GetGameH() };
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.IniFilename = nullptr;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

        // Keyboard mapping
        io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
        io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
        io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
        io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
        io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
        io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
        io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
        io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
        io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
        io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
        io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
        io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
        io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
        io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
        io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
        io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
        io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
        io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

        // Create pyxie renderer pipeline
        createDeviceObjects();

        // Refresh font texture
        refreshFontTexture();
    }

    bool Editor::handleEventImGUI(const SDL_Event* event)
    {
        ImGuiIO& io = ImGui::GetIO();
        switch (event->type)
        {
            case SDL_MOUSEWHEEL:
            {
                if (event->wheel.x > 0) io.MouseWheelH += 1;
                if (event->wheel.x < 0) io.MouseWheelH -= 1;
                if (event->wheel.y > 0) io.MouseWheel += 1;
                if (event->wheel.y < 0) io.MouseWheel -= 1;
                return true;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                return true;
            }
            case SDL_TEXTINPUT:
            {
                io.AddInputCharactersUTF8(event->text.text);
                return true;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                int key = event->key.keysym.scancode;
                IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
                io.KeysDown[key] = (event->type == SDL_KEYDOWN);
                io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
                io.KeySuper = false;
                return true;
            }            
        }
        return false;
    }

    void Editor::updateImGUI()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Update camera and display
        io.DisplaySize = { pyxieSystemInfo::Instance().GetGameW(), pyxieSystemInfo::Instance().GetGameH() };
        m_camera->SetScreenOffset(Vec2(-pyxieSystemInfo::Instance().GetGameW(), pyxieSystemInfo::Instance().GetGameH()));

        // Update mouse position and button states
        int mouse_x_local, mouse_y_local;
        Uint32 mouse_buttons = SDL_GetMouseState(&mouse_x_local, &mouse_y_local);
        io.MouseDown[0] = (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
        io.MouseDown[1] = (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
        io.MouseDown[2] = (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;

        SDL_Window* window = (SDL_Window*)m_app->getAppWindow();
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_INPUT_FOCUS)
        {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            io.MousePos = ImVec2((mouse_x_local) * pyxieSystemInfo::Instance().GetGameW() / w, (mouse_y_local)*pyxieSystemInfo::Instance().GetGameH() / h);
        }
    }

    void Editor::renderImGUI()
    {
        ImGuiIO& io = ImGui::GetIO();
        int fb_width = io.DisplaySize.x * io.DisplayFramebufferScale.x;
        int	fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
        if (fb_width == 0 || fb_height == 0) return;

        ImDrawData* imGuiData = ImGui::GetDrawData();
        if (!imGuiData) return;

        imGuiData->ScaleClipRects(io.DisplayFramebufferScale);
        m_editableFigure->ClearAllMeshes();

        for (int n = 0; n < imGuiData->CmdListsCount; n++)
        {
            int idx_buffer_offset = 0;
            std::string mesh_name = "m" + std::to_string(n);
            m_editableFigure->AddMesh(mesh_name.c_str(), "mate01");

            int index = m_editableFigure->GetMeshIndex(GenerateNameHash(mesh_name.c_str()));

            VertexAttribute attribute[3];
            attribute[0] = { ATTRIBUTE_ID_POSITION , false, 2, GL_FLOAT };
            attribute[1] = { ATTRIBUTE_ID_UV0 , false, 2, GL_FLOAT };
            attribute[2] = { ATTRIBUTE_ID_COLOR , true, 4, GL_UNSIGNED_BYTE };
            m_editableFigure->SetMeshVertexAttributes(index, attribute, 3);

            const ImDrawList* cmd_list = imGuiData->CmdLists[n];
            m_editableFigure->SetMeshVertices(index, cmd_list->VtxBuffer.Data, (uint32_t)(cmd_list->VtxBuffer.Size));
            m_editableFigure->SetMeshIndices(index, 0, (uint32_t*)cmd_list->IdxBuffer.Data, (uint32_t)(cmd_list->IdxBuffer.Size / 3), 2);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];			
                
                m_editableFigure->AddMeshDrawSet(index, idx_buffer_offset / 3, pcmd->ElemCount / 3);

                auto result = m_assetSamplers.find((uint32_t)pcmd->TextureId);
                if (result != m_assetSamplers.end())
                {
                    m_editableFigure->AddMeshDrawSetParam(index, cmd_i, "ColorSampler", &result->second, ShaderParameterDataType::ParamTypeSampler);
                }
                else if(m_assetSamplers.size() > 0)
                {
                    m_editableFigure->AddMeshDrawSetParam(index, cmd_i, "ColorSampler", &m_assetSamplers[m_fontTexture->GetTextureHandle()], ShaderParameterDataType::ParamTypeSampler);
                }

                uint32_t scissor_test_enable[4] = { 1, 0, 0, 0 };
                m_editableFigure->AddMeshDrawSetState(index, cmd_i, (ShaderParameterKey)pyxieRenderContext::Instance().GetShaderParameterInfoByName("scissor_test_enable")->key, scissor_test_enable);

                float scissor[4] = { pcmd->ClipRect.x, fb_height - pcmd->ClipRect.w, pcmd->ClipRect.z - pcmd->ClipRect.x, pcmd->ClipRect.w - pcmd->ClipRect.y };
                m_editableFigure->AddMeshDrawSetState(index, cmd_i, (ShaderParameterKey)pyxieRenderContext::Instance().GetShaderParameterInfoByName("scissor")->key, scissor);

                uint32_t blend_enable[4] = { 1, 0, 0, 0 };
                m_editableFigure->AddMeshDrawSetState(index, cmd_i, (ShaderParameterKey)pyxieRenderContext::Instance().GetShaderParameterInfoByName("blend_enable")->key, blend_enable);

                uint32_t cull_face_enable[4] = { 0, 0, 0, 0 };
                m_editableFigure->AddMeshDrawSetState(index, cmd_i, (ShaderParameterKey)pyxieRenderContext::Instance().GetShaderParameterInfoByName("cull_face_enable")->key, cull_face_enable);

                uint32_t depth_test_enable[4] = { 0, 0, 0, 0 };
                m_editableFigure->AddMeshDrawSetState(index, cmd_i, (ShaderParameterKey)pyxieRenderContext::Instance().GetShaderParameterInfoByName("depth_test_enable")->key, depth_test_enable);

                idx_buffer_offset += pcmd->ElemCount * sizeof(ImDrawIdx);
            }
        }
    }

}
