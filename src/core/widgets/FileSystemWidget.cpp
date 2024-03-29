#include "core/plugin/DragDropPlugin.h"
#include "core/FileHandle.h"
#include "core/Editor.h"
#include "core/EditorUtils.h"
#include "core/task/TaskManager.h"

#include "core/widgets/FileSystemWidget.h"

#include <SDL.h>
#include <array>
#include <string>

#include <imgui.h>
#include <imgui_internal.h>

constexpr int ICON_SIZE = 64.f;

namespace ImGui
{
    void RenderFrameEx(ImVec2 p_min, ImVec2 p_max, bool border, float rounding, float thickness)
    {
        ImGuiWindow *window = GetCurrentWindow();
        if (border)
        {
            window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow), rounding, 15, thickness);
            window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border), rounding, 15, thickness);
        }
    }

    bool ImageButtonWithAspectAndTextDOWN(ImTextureID texId, const std::string &name,
                                          const ImVec2 &texture_size, const ImVec2 &imageSize,
                                          const ImVec2 &uv0 = ImVec2(0, 0),
                                          const ImVec2 &uv1 = ImVec2(1, 1), int frame_padding = -1,
                                          const ImVec4 &bg_col = ImVec4(0, 0, 0, 0),
                                          const ImVec4 &tint_col = ImVec4(1, 1, 1, 1))
    {
        ImGuiWindow *window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImVec2 size = imageSize;
        if (size.x <= 0 && size.y <= 0)
        {
            size.x = size.y = ImGui::GetTextLineHeightWithSpacing();
        }
        else
        {
            if (size.x <= 0)
                size.x = size.y;
            else if (size.y <= 0)
                size.y = size.x;
            size *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
        }

        ImGuiContext &g = *GImGui;
        const ImGuiStyle &style = g.Style;

        ImVec2 size_name = ImGui::CalcTextSize(name.c_str(), nullptr, true);
        auto name_sz = name.size();
        std::string label_str = name;

        if (size_name.x > imageSize.x)
        {
            for (auto ds = name_sz; ds > 3; --ds)
            {
                size_name = ImGui::CalcTextSize(label_str.c_str(), nullptr, true);
                if (size_name.x < imageSize.x)
                {
                    label_str[ds - 2] = '.';
                    label_str[ds - 1] = '.';
                    label_str[ds] = '.';
                    break;
                }

                label_str.pop_back();
            }
        }

        const char *label = label_str.c_str();

        const ImGuiID id = window->GetID(label);
        const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
        const bool hasText = textSize.x > 0;

        const float innerSpacing =
            hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
        const ImVec2 padding =
            (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
        bool istextBig = false;
        if (textSize.x > imageSize.x)
        {
            istextBig = true;
        }
        const ImVec2 totalSizeWithoutPadding(size.x, size.y > textSize.y ? size.y : textSize.y);

        ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2);
        ImVec2 start(0, 0);
        start = window->DC.CursorPos + padding;
        if (size.y < textSize.y)
        {
            start.y += (textSize.y - size.y) * .5f;
        }
        ImVec2 reajustMIN(0, 0);
        ImVec2 reajustMAX = size;
        if (bb.Max.y - textSize.y < start.y + reajustMAX.y)
        {
            reajustMIN.x += textSize.y / 2;
            reajustMAX.x -= textSize.y / 2;
            reajustMAX.y -= textSize.y;
        }
        ImRect image_bb(start + reajustMIN, start + reajustMAX);
        start = window->DC.CursorPos + padding;
        start.y += (size.y - textSize.y) + 2;
        if (istextBig == false)
        {
            start.x += (size.x - textSize.x) * .5f;
        }

        ItemSize(bb);
        if (!ItemAdd(bb, id))
            return false;

        bool hovered = false, held = false;
        bool pressed = ButtonBehavior(bb, id, &hovered, &held);

        // Render
        ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
        RenderFrame(bb.Min, bb.Max, col, true,
                    ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));
        if (bg_col.w > 0.0f)
            window->DrawList->AddRectFilled(image_bb.Min, image_bb.Max, GetColorU32(bg_col));

        float w = texture_size.x;
        float h = texture_size.y;
        ImVec2 imgSz = ImVec2(image_bb.GetWidth(), image_bb.GetHeight());
        float max_size = ImMax(imgSz.x, imgSz.y);
        float aspect = w / h;
        if (w > h)
        {
            float m = ImMin(max_size, w);

            imgSz.x = m;
            imgSz.y = m / aspect;
        }
        else if (h > w)
        {
            float m = ImMin(max_size, h);

            imgSz.x = m * aspect;
            imgSz.y = m;
        }

        if (imgSz.x > imgSz.y)
            image_bb.Min.y += (max_size - imgSz.y) * 0.5f;
        if (imgSz.x < imgSz.y)
            image_bb.Min.x += (max_size - imgSz.x) * 0.5f;

        image_bb.Max = image_bb.Min + imgSz;
        window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

        if (textSize.x > 0)
        {
            ImGui::RenderText(start, label);
        }
        return pressed;
    }
} // namespace ImGui

namespace ige::creator
{
    FileSystemWidget::FileSystemWidget()
        : m_isDirty(false)
    {
        m_iconTextures["folder"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/folder").c_str());
        m_iconTextures["image"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_image").c_str());
        m_iconTextures["file"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_undefined").c_str());
        m_iconTextures["prefab"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_prefab").c_str());
        m_iconTextures["python"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_python").c_str());
        m_iconTextures["model"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_model").c_str());
        m_iconTextures["font"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_font").c_str());
        m_iconTextures["audio"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_audio").c_str());
        m_iconTextures["effect"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_effect").c_str());
        m_iconTextures["animation_clip"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_animation_clip").c_str());
        m_iconTextures["animator"] = ResourceCreator::Instance().NewTexture(GetEnginePath("icons/file_animator").c_str());

        m_cache.set_scan_frequency(std::chrono::milliseconds(1000));
    }

    FileSystemWidget::FileSystemWidget(const fs::directory_cache &cache)
        : m_cache(cache), m_isDirty(false)
    {
    }

    FileSystemWidget::~FileSystemWidget()
    {
        for (auto icon : m_iconTextures)
        {
            icon.second->DecReference();
        }
        m_iconTextures.clear();
    }

    void FileSystemWidget::_drawImpl()
    {
        if (m_isDirty)
        {
            const auto root_path = fs::current_path();
            std::error_code err;
            if (m_root != root_path || !fs::exists(m_cache.get_path(), err))
            {
                m_root = root_path;
                m_cache.set_path(m_root);
            }
            m_hierarchy.clear();
            m_hierarchy = fs::split_until(m_cache.get_path(), m_root);
            m_selection = "";
            m_isDirty = false;
        }

        const float size = ICON_SIZE;

        int id = 0;
        for (const auto &dir : m_hierarchy)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0, 1.0, 1.0, 1.0), "/");
            ImGui::SameLine();

            ImGui::PushID(id++);
            bool clicked = ImGui::Button(dir.filename().string().c_str());
            ProcessDragDropTarget(dir);
            ImGui::PopID();

            if (clicked)
            {
                SetCachePath(dir);
                m_isDirty = true;
                break;
            }
        }

        ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 100.f);
        auto lastHidden = m_bShowHidden;
        ImGui::Checkbox("Show Hidden", &m_bShowHidden);
        if (m_bShowHidden && !lastHidden) {
            m_cache.refresh();
        }

        auto entries = m_cache.entries();
        if (!m_bShowHidden) {
            for (auto itr = entries.begin(); itr != entries.end();) {
                const auto& name = (*itr).stem;
                const auto& ext = (*itr).extension;
                if (IsFormat(E_FileExts::Hidden, name) || IsFormat(E_FileExts::Hidden, ext)) {
                    itr = entries.erase(itr);
                }
                else {
                    itr++;
                }
            }
        }

        static char inputName[256] = {};
        memset(inputName, 0, 256);
        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::BeginMenu(("New Folder" + getIdAsString()).c_str())) {
                if (ImGui::InputText(("##NAME_0" + getIdAsString()).c_str(), inputName, 256,
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
                    try {
                        if (strlen(inputName) > 0) {
                            auto path = fs::path(m_cache.get_path()).append(inputName);
                            fs::create_directory(path);
                        }
                    } catch (std::exception e) {}
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(("New Script" + getIdAsString()).c_str())) {
                if (ImGui::InputText(("##NAME_1" + getIdAsString()).c_str(), inputName, 256,
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
                    if (strlen(inputName) > 0)
                        Editor::getInstance()->createScript(m_cache.get_path().string(), std::string(inputName));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(("New Animator" + getIdAsString()).c_str())) {
                if (ImGui::InputText(("##NAME_2" + getIdAsString()).c_str(), inputName, 256,
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
                    if (strlen(inputName) > 0)
                        Editor::getInstance()->createAnimator(m_cache.get_path().string(), std::string(inputName));
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();

        fs::path current_path = m_cache.get_path();

        const auto is_selected = [&](const fs::path &_path) {
            bool is_selected = m_selection == _path;
            return is_selected;
        };
        auto process_cache_entry = [&](const auto &cache_entry) {
            const auto &absolute_path = cache_entry.entry.path();
            const auto &name = cache_entry.stem;
            const auto &relative = cache_entry.protocol_path;
            const auto &file_ext = cache_entry.extension;
            const std::string &filename = cache_entry.filename;

            const auto on_rename = [&](const std::string &new_name) {
                fs::path new_absolute_path = absolute_path;
                new_absolute_path.remove_filename();
                new_absolute_path /= new_name;
                std::error_code err;
                fs::rename(absolute_path, new_absolute_path, err);
                m_cache.refresh();
            };

            const auto on_delete = [&]() {
                std::error_code err;
                fs::remove(absolute_path, err);
                m_selection = "";
                m_cache.refresh();
            };

            const auto on_click = [&]() {
                m_selection = absolute_path;
            };

            // Do not show hidden file/folder
            if (!m_bShowHidden && IsFormat(E_FileExts::Hidden, name))
                return;

            if (fs::is_directory(cache_entry.entry.status()))
            {
                const auto on_double_click = [&]() {
                    current_path = absolute_path;
                    m_isDirty = true;
                };

                DrawEntry(m_iconTextures["folder"], false, name, absolute_path, is_selected(absolute_path),
                          size, on_click, on_double_click, on_rename, on_delete);
                return;
            }
            else //file
            {
                // Do not show hidden files
                if (!m_bShowHidden && IsFormat(E_FileExts::Hidden, file_ext))
                    return;

                auto icon = m_iconTextures["file"];
                if (IsFormat(E_FileExts::Script, file_ext))
                {
                    icon = m_iconTextures["python"];
                }
                else if (IsFormat(E_FileExts::Figure, file_ext))
                {
                    icon = m_iconTextures["model"];
                }
                else if (IsFormat(E_FileExts::Sprite, file_ext))
                {
                    icon = m_iconTextures["image"];
                }
                else if (IsFormat(E_FileExts::Font, file_ext))
                {
                    icon = m_iconTextures["font"];
                }
                else if (IsFormat(E_FileExts::Audio, file_ext))
                {
                    icon = m_iconTextures["audio"];
                }
                else if (IsFormat(E_FileExts::Prefab, file_ext))
                {
                    icon = m_iconTextures["prefab"];
                }
                else if (IsFormat(E_FileExts::Particle, file_ext))
                {
                    icon = m_iconTextures["effect"];
                }
                else if (IsFormat(E_FileExts::AnimationClip, file_ext))
                {
                    icon = m_iconTextures["animation_clip"];
                }
                else if (IsFormat(E_FileExts::Animator, file_ext))
                {
                    icon = m_iconTextures["animator"];
                }

                const auto on_double_click = [&]() {
                    if (IsFormat(E_FileExts::Scene, file_ext))
                    {
                        TaskManager::getInstance()->addTask([&]() {
                            Editor::getInstance()->loadScene(absolute_path.string());
                        });
                    }
                    else if (IsFormat(E_FileExts::Prefab, file_ext))
                    {
                        TaskManager::getInstance()->addTask([&]() {
                            Editor::getInstance()->openPrefab(absolute_path.string());
                        });
                    }
                    else if (IsFormat(E_FileExts::Animator, file_ext))
                    {
                        TaskManager::getInstance()->addTask([&]() {
                            Editor::getInstance()->openAnimator(absolute_path.string());
                        });
                    }
                    else
                    {
                        EditorUtils::openFile(absolute_path);
                    }
                };

                DrawEntry(icon, false, filename, absolute_path, is_selected(absolute_path),
                          size, on_click, on_double_click, on_rename, on_delete);
            }
        };

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
        const auto &style = ImGui::GetStyle();
        auto avail = ImGui::GetWindowContentRegionWidth();
        auto item_size = size + style.ItemSpacing.x;
        auto items_per_line_exact = avail / item_size;
        auto items_per_line_floor = ImFloor(items_per_line_exact);
        auto count = entries.size();
        auto items_per_line = std::min(size_t(items_per_line_floor), count);
        auto extra = ((items_per_line_exact - items_per_line_floor) * item_size) /
                     std::max(1.0f, items_per_line_floor - 1);
        auto lines = std::max<int>(1, int(ImCeil(float(count) / float(items_per_line))));
        ImGuiListClipper clipper(lines, size);

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                auto start = i * items_per_line;
                auto end = start + std::min(count - start, items_per_line);
                for (int j = start; j < end; ++j)
                {
                    const auto &cache_entry = entries[j];

                    ImGui::PushID(int(j));
                    process_cache_entry(cache_entry);
                    ImGui::PopID();

                    if (j != end - 1)
                    {
                        ImGui::SameLine(0.0f, style.ItemSpacing.x + extra);
                    }
                }
                ImGui::NewLine();
            }
        }
        ImGui::PopStyleVar();

        SetCachePath(current_path);

        // Drag & drop zone
        auto window = ImGui::GetCurrentWindow();
        if (ImGui::BeginDragDropTargetCustom(window->InnerClipRect, window->ID))
        {
            ImGuiDragDropFlags flags = 0;
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::to_string((int)EDragDropID::OBJECT).c_str(), flags))
            {
                Editor::getInstance()->savePrefab(*(uint64_t*)payload->Data, current_path.string());
            }
            ImGui::EndDragDropTarget();
        }
    }

    void FileSystemWidget::SetCachePath(const fs::path &path)
    {
        m_cache.set_path(path);
    }

    bool FileSystemWidget::DrawEntry(pyxieTexture *icon, bool is_loading, const std::string &name,
                                     const fs::path &absolute_path, bool is_selected, const float size,
                                     const std::function<void()> &on_click, const std::function<void()> &on_double_click,
                                     const std::function<void(const std::string &)> &on_rename,
                                     const std::function<void()> &on_delete)
    {
        enum class entry_action
        {
            none,
            clicked,
            double_clicked,
            explored,
            renamed,
            deleted
        };

        bool is_popup_opened = false;
        entry_action action = entry_action::none;

        bool open_rename_menu = false;

        ImGui::PushID(name.c_str());
        if (is_selected && !ImGui::IsAnyItemActive() && ImGui::IsWindowFocused())
        {
            if (ImGui::IsKeyPressed(SDL_SCANCODE_F2))
            {
                open_rename_menu = true;
            }

            if (ImGui::IsKeyPressed(SDL_SCANCODE_DELETE))
            {
                action = entry_action::deleted;
            }
        }

        ImVec2 item_size = {size, size};
        ImVec2 texture_size = item_size;

        static const ImVec2 uv0 = {0.0f, 1.0f};
        static const ImVec2 uv1 = {1.0f, 0.0f};

        auto col = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(col.x, col.y, col.z, 0.44f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(col.x, col.y, col.z, 0.86f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(col.x, col.y, col.z, 1.0f));

        const int padding = 1.f;
        auto pos = ImGui::GetCursorScreenPos();
        pos.y += item_size.y + padding * 2.0f;

        if (ImGui::ImageButtonWithAspectAndTextDOWN((ImTextureID)icon->GetTextureHandle(), name, texture_size, item_size, uv0, uv1, padding))
        {
            action = entry_action::clicked;
        }

        ImGui::PopStyleColor(3);

        if (ImGui::IsItemHovered())
        {
            if (on_double_click)
            {
                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            }
            if (ImGui::IsMouseDoubleClicked(0))
            {
                action = entry_action::double_clicked;
            }
            ImGui::SetTooltip(name.c_str());
        }

        if (is_selected && ImGui::GetNavInputAmount(ImGuiNavInput_Input, ImGuiInputReadMode_Pressed) > 0.0f)
        {
            action = entry_action::double_clicked;
        }

        std::array<char, 64> input_buff;
        input_buff.fill(0);
        auto name_sz = std::min(name.size(), input_buff.size() - 1);
        std::memcpy(input_buff.data(), name.c_str(), name_sz);

        if (ImGui::BeginPopupContextItem("ENTRY_CONTEXT_MENU"))
        {
            is_popup_opened = true;

            if (ImGui::MenuItem("Open", "Ctrl + O"))
            {
                action = entry_action::double_clicked;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Explore", "Ctrl + E"))
            {
                action = entry_action::explored;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Rename", "F2"))
            {
                open_rename_menu = true;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Delete", "DEL"))
            {
                action = entry_action::deleted;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (open_rename_menu)
        {
            ImGui::OpenPopup("ENTRY_RENAME_MENU");
            ImGui::SetNextWindowPos(pos);
        }

        if (ImGui::BeginPopup("ENTRY_RENAME_MENU"))
        {
            is_popup_opened = true;
            if (open_rename_menu)
            {
                ImGui::SetKeyboardFocusHere();
            }
            ImGui::PushItemWidth(150.0f);
            if (ImGui::InputText("##NAME", input_buff.data(), input_buff.size(),
                                 ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
            {
                action = entry_action::renamed;
                ImGui::CloseCurrentPopup();
            }

            if (open_rename_menu)
            {
                ImGui::ActivateItem(ImGui::GetItemID());
            }
            ImGui::PopItemWidth();
            ImGui::EndPopup();
        }
        if (is_selected)
        {
            ImGui::RenderFrameEx(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true, 0.0f, 1.0f);
        }

        if (is_loading)
        {
            action = entry_action::none;
        }

        switch (action)
        {
        case entry_action::clicked:
        {
            if (on_click)
            {
                on_click();
            }
            getOnClickEvent().invoke(this);
        }
        break;
        case entry_action::double_clicked:
        {
            if (on_double_click)
            {
                on_double_click();
            }
        }
        break;
        case entry_action::renamed:
        {
            std::string new_name = std::string(input_buff.data());
            if (new_name != name && !new_name.empty())
            {
                if (on_rename)
                {
                    on_rename(new_name);
                }
            }
        }
        break;

        case entry_action::deleted:
        {
            if (on_delete)
            {
                on_delete();
            }
        }
        break;

        case entry_action::explored:
        {
            EditorUtils::openExplorer(absolute_path);
        }
        break;

        default:
            break;
        }

        if (!ProcessDragDropSource(icon, absolute_path))
        {
            ProcessDragDropTarget(absolute_path);
        }

        ImGui::PopID();
        return is_popup_opened;
    }

    bool FileSystemWidget::ProcessDragDropSource(pyxieTexture *preview, const fs::path &path)
    {
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            const auto filename = path.filename();
            std::string extension = filename.has_extension() ? filename.extension().string() : "folder";
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            ImGui::TextUnformatted(filename.string().c_str());
            static std::string relative; // Keep content in memory
            relative = fs::relative(path).string();
            std::replace(relative.begin(), relative.end(), '\\', '/');
            ImGui::SetDragDropPayload(extension.c_str(), &relative, sizeof(relative));
            ImGui::EndDragDropSource();
            return true;
        }
        return false;
    }

    void FileSystemWidget::ProcessDragDropTarget(const fs::path &absolute_path)
    {
        if (ImGui::BeginDragDropTarget())
        {
            ImGuiDragDropFlags flags = 0;
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(std::to_string((int)EDragDropID::OBJECT).c_str(), flags))
            {
                auto relative = fs::relative(absolute_path).string();
                std::replace(relative.begin(), relative.end(), '\\', '/');
                Editor::getInstance()->savePrefab(*(uint64_t*)payload->Data, relative);
            }
            ImGui::EndDragDropTarget();
        }
    }
} // namespace ige::creator
