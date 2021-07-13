#include <imgui.h>

#include "core/panels/subpanels/BitmapFontCreator.h"
#include "core/widgets/Widgets.h"
#include "core/widgets/TextArea.h"

#include <core/widgets/Image.h>

#include <core/FileHandle.h>
#include <core/plugin/DragDropPlugin.h>
#include <core/dialog/OpenFileDialog.h>

#include <cstdlib>

#include "utils/filesystem.h"
namespace fs = ghc::filesystem;

namespace ige::creator
{
    static const int LIMIT_GLYPH_PER_PAGE = 5;

    static std::wstring utf8toUtf16(const std::string& str)
    {
        if (str.empty())
            return std::wstring();

        size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
            str.data(), (int)str.size(), NULL, 0);
        if (charsNeeded == 0)
            return std::wstring();

        std::vector<wchar_t> buffer(charsNeeded);
        int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
            str.data(), (int)str.size(), &buffer[0], buffer.size());
        if (charsConverted == 0)
            return std::wstring();

        return std::wstring(&buffer[0], charsConverted);
    }

    static std::string utf16toUtf8(const std::wstring& wstr)
    {
        std::string retStr;
        if (!wstr.empty())
        {
            int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

            if (sizeRequired > 0)
            {
                std::vector<char> utf8String(sizeRequired);
                int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
                    -1, &utf8String[0], utf8String.size(), NULL,
                    NULL);
                if (bytesConverted != 0)
                {
                    retStr = &utf8String[0];
                }
                else
                {
                    std::stringstream err;
                    err << __FUNCTION__
                        << " std::string WstrToUtf8Str failed to convert wstring '"
                        << wstr.c_str() << L"'";
                    throw std::runtime_error(err.str());
                }
            }
        }
        return retStr;
    }

    static std::string removeSpaces(std::string input)
    {
        input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
        input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
        return input;
    }

    static std::vector<std::string> splitpath(const std::string& str, const std::set<char> delimiters)
    {
        std::vector<std::string> result;
        char const* pch = str.c_str();
        char const* start = pch;
        for (; *pch; ++pch)
        {
            if (delimiters.find(*pch) != delimiters.end())
            {
                if (start != pch)
                {
                    std::string str(start, pch);
                    result.push_back(str);
                }
                else
                {
                    result.push_back("");
                }
                start = pch + 1;
            }
        }
        result.push_back(start);

        return result;
    }


    BitmapGlyphEditor::BitmapGlyphEditor()
    {

    }

    BitmapGlyphEditor::~BitmapGlyphEditor()
    {
        Clear();
    }

    void BitmapGlyphEditor::Clear()
    {
        if (m_glyphGroup)
        {
            m_glyphGroup->removeAllWidgets();
            m_glyphGroup->removeAllPlugins();
            m_glyphGroup = nullptr;
        }

        if (m_data)
            m_data = nullptr;
    }

    void BitmapGlyphEditor::Draw(std::shared_ptr<BitmapGlyph> data, std::shared_ptr<Group> parent)
    {
        Clear();
        if (parent == nullptr || data == nullptr) return;
        m_data = data;
        m_glyphGroup = parent->createWidget<Group>("Glyph", false, false);

        m_glyphGroup->createWidget<TextField>("Index", std::to_string(data->ID), true);
        std::wstring ws(1, data->Unicode);
        std::string s = utf16toUtf8(ws);
        m_glyphGroup->createWidget<TextField>("Unicode", s, true);
        
        std::array posD = { data->x, data->y };
        m_glyphGroup->createWidget<Drag<float, 2>>("Position", ImGuiDataType_Float, posD, 0.1f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto data = this->getData();
            data->x = val[0];
            data->y = val[1];
            this->setData(data);
            });

        std::array sizeD = { data->w, data->h };
        m_glyphGroup->createWidget<Drag<float, 2>>("Size", ImGuiDataType_Float, sizeD, 1.0f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto data = this->getData();
            data->w = val[0];
            data->h = val[1];
            this->setData(data);
            });

        std::array offsetD = { data->xoffset, data->yoffset};
        m_glyphGroup->createWidget<Drag<float, 2>>("Offset", ImGuiDataType_Float, offsetD, 1.0f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto data = this->getData();
            data->xoffset = val[0];
            data->yoffset = val[1];
            this->setData(data);
            });

        std::array advanceD = { data->xadvance};
        m_glyphGroup->createWidget<Drag<float, 1>>("Advance", ImGuiDataType_Float, advanceD, 1.0f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            auto data = this->getData();
            data->xadvance = val[0];
            this->setData(data);
            });
    }

    void BitmapGlyphEditor::setData(std::shared_ptr<BitmapGlyph> data)
    {
        m_data = data;
    }


    //!
    BitmapFontCreator::BitmapFontCreator(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_texture = nullptr;
        m_CurrentPageIndex = 0;
    }
    
    BitmapFontCreator::~BitmapFontCreator()
    {
        clear();
    }

    void BitmapFontCreator::initialize()
    {
        clear();

        m_settings.closable = true;

        m_topGroup = createWidget<Group>("BitmapFontCreator Top", false, false);

        m_fontGroup = createWidget<Group>("BitmapFontCreator Log", false, false);

        dirty();
    }

    void BitmapFontCreator::_drawImpl()
    {
        //Panel::_drawImpl();

        if (isOpened())
        {
            int windowFlags = ImGuiWindowFlags_None;

            if (m_settings.hideTitle)					windowFlags |= ImGuiWindowFlags_NoTitleBar;
            if (!m_settings.resizable)					windowFlags |= ImGuiWindowFlags_NoResize;
            if (!m_settings.movable)					windowFlags |= ImGuiWindowFlags_NoMove;
            if (!m_settings.dockable)					windowFlags |= ImGuiWindowFlags_NoDocking;
            if (m_settings.hideBackground)				windowFlags |= ImGuiWindowFlags_NoBackground;
            if (m_settings.forceHorizontalScrollbar)	windowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
            if (m_settings.forceVerticalScrollbar)		windowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
            if (m_settings.allowHorizontalScrollbar)	windowFlags |= ImGuiWindowFlags_HorizontalScrollbar;
            if (!m_settings.bringToFrontOnFocus)		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            if (!m_settings.collapsable)				windowFlags |= ImGuiWindowFlags_NoCollapse;
            if (!m_settings.allowInputs)				windowFlags |= ImGuiWindowFlags_NoInputs;

            ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            ImVec2 vMax = ImGui::GetWindowContentRegionMax();

            vMin.x += ImGui::GetWindowPos().x;
            vMin.y += ImGui::GetWindowPos().y;

            vMax.x += ImGui::GetWindowPos().x;
            vMax.y += ImGui::GetWindowPos().y;

            ImGui::SetNextWindowSizeConstraints(vMin, vMax);

            if (ImGui::Begin((m_name).c_str(), m_settings.closable ? &m_bIsOpened : nullptr, windowFlags))
            {
                m_bIsHovered = ImGui::IsWindowHovered();
                m_bIsFocused = ImGui::IsWindowFocused();

                if (!m_bIsOpened) m_closeEvent.invoke();

                auto windowPos = ImGui::GetWindowPos();
                if (m_position.x != windowPos.x || m_position.y != windowPos.y)
                {
                    getOnPositionChangedEvent().invoke(windowPos);
                    m_position = windowPos;
                }

                auto newSize = ImGui::GetContentRegionAvail();
                if (m_size.x != newSize.x || m_size.y != newSize.y)
                {
                    getOnSizeChangedEvent().invoke(newSize);
                    m_size = newSize;
                }

                // draw top group
                if(m_topGroup) m_topGroup->draw();
                // draw scrollable group
                ImGui::BeginChild("BitmapFontCreator");
                if(m_fontGroup)
                    m_fontGroup->draw();
                ImGui::EndChild();
            }
            ImGui::End();
        }
    }

    void BitmapFontCreator::clear()
    {
        if (m_imgWidget) {
            m_imgWidget->removeAllPlugins();
            m_imgWidget = nullptr;
        }

        if (m_texture != nullptr) {
            m_texture->DecReference();
            m_texture = nullptr;
        }

        if (m_fontPageGroup)
        {
            m_fontPageGroup->removeAllWidgets();
            m_fontPageGroup->removeAllPlugins();
            m_fontPageGroup = nullptr;
        }
        
        if (m_GroupLeft)
        {
            m_GroupLeft->removeAllWidgets();
            m_GroupLeft->removeAllPlugins();
            m_GroupLeft = nullptr;
        }

        if (m_GroupRight)
        {
            m_GroupRight->removeAllWidgets();
            m_GroupRight->removeAllPlugins();
            m_GroupRight = nullptr;
        }
        
        if (m_columns)
        {
            m_columns->removeAllWidgets();
            m_columns->removeAllPlugins();
            m_columns = nullptr;
        }

        if (m_fontGroup)
        {
            m_fontGroup->removeAllWidgets();
            m_fontGroup->removeAllPlugins();
            m_fontGroup = nullptr;
        }
        
        removeAllWidgets();

        if (m_font != nullptr) {
            m_font = nullptr;
        }

        if (m_glyphList.size() > 0) {
            int len = m_glyphList.size();
            for (int i = 0; i < len; i++)
            {
                m_glyphList[i]->Clear();
                m_glyphList[i] = nullptr;
            }
            m_glyphList.clear();
        }
    }

    void BitmapFontCreator::update(float dt)
    {
        Panel::update(dt);

        if (m_bPanelFlag)
            _drawPanel();
        if (m_bPageFlag)
            _drawPage(m_CurrentPageIndex);
        if (m_bTextureFlag)
            _drawTexture();
    }

    void BitmapFontCreator::_drawPanel()
    {
        if (!m_bPanelFlag) return;
        if (m_fontGroup == nullptr) return;
        
        //! Clear Widgets
        if (m_fontPageGroup)
        {
            m_fontPageGroup->removeAllWidgets();
            m_fontPageGroup->removeAllPlugins();
            m_fontPageGroup = nullptr;
        }
        
        if (m_GroupLeft)
        {
            m_GroupLeft->removeAllWidgets();
            m_GroupLeft->removeAllPlugins();
            m_GroupLeft = nullptr;
        }

        if (m_GroupRight)
        {
            m_GroupRight->removeAllWidgets();
            m_GroupRight->removeAllPlugins();
            m_GroupRight = nullptr;
        }

        if (m_columns)
        {
            m_columns->removeAllWidgets();
            m_columns->removeAllPlugins();
            m_columns = nullptr;
        }

        m_fontGroup->removeAllWidgets();
        m_fontGroup->removeAllPlugins();


        m_bPanelFlag = false;

        m_columns = m_fontGroup->createWidget<Columns<2>>(300.f);
        m_GroupLeft = m_columns->createWidget<Group>("G1", false, false);
        m_GroupRight = m_columns->createWidget<Group>("G2", false, false);


        auto txtPath = m_GroupLeft->createWidget<TextField>("Path", m_path);
        txtPath->setEndOfLine(false);
        txtPath->getOnDataChangedEvent().addListener([this](auto txt) {
            this->onLoadImg(txt);
            });

        for (const auto& type : GetFileExtensionSuported(E_FileExts::Sprite))
        {
            txtPath->addPlugin<DDTargetPlugin<std::string>>(type)->getOnDataReceivedEvent().addListener([this](auto txt) {
                this->onLoadImg(txt);
                dirty();
                });
        }

        auto btnB1 = m_GroupLeft->createWidget<Button>("Browse", ImVec2(64.f, 0.f));
        btnB1->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Assets", "", { "Texture (*.pyxi)", "*.pyxi" }).result();
            if (files.size() > 0)
            {
                this->onLoadImg(files[0]);
                dirty();
            }
            });
        btnB1->setEndOfLine(true);

        m_GroupLeft->createWidget<Button>("Generate Character Set", ImVec2(256.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto s = this->getChacterCode();
            this->generateCode(s);
            });

        m_GroupLeft->createWidget<Button>("Load Bitmap Font", ImVec2(256.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            auto files = OpenFileDialog("Import Bitmap Font", "", { "Bitmap (*.pybm)", "*.pybm" }).result();
            if (files.size() > 0)
            {
                bool kq = this->loadBitmapFont(files[0]);
                if (kq) {
                    this->onLoadImg(m_path);
                    dirty();
                    m_bPageFlag;
                }
            }
            });

        m_GroupLeft->createWidget<Button>("Save FontBitmap", ImVec2(256.f, 0.f))->getOnClickEvent().addListener([this](auto widget) {
            this->saveBitmapFont();
            });

        m_GroupLeft->createWidget<Label>("Character set");
        auto txtChars = m_GroupLeft->createWidget<TextArea>("", "", ImVec2(256.f, 128.f));
        txtChars->getOnDataChangedEvent().addListener([this](auto text) {
            this->setChacterCode(text);
            });

        m_fontPageGroup = m_GroupLeft->createWidget<Group>("Page", false, false);
    }

    void BitmapFontCreator::_drawPage(int pageIndex)
    {
        if (!m_bPageFlag) return;
        if (m_fontPageGroup == nullptr || m_font == nullptr) return;
        m_fontPageGroup->removeAllPlugins();
        m_fontPageGroup->removeAllWidgets();
        
        std::array sizeD = { (float)m_font->getSize() };
        m_fontPageGroup->createWidget<Drag<float, 1>>("Font Size", ImGuiDataType_Float, sizeD, 1.0f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            m_font->setSize((int)val[0]);
            });

        std::array baseD = { (float)m_font->getBaseSize() };
        m_fontPageGroup->createWidget<Drag<float, 1>>("Font Base Size", ImGuiDataType_Float, baseD, 1.0f, 0.f)->getOnDataChangedEvent().addListener([this](auto val) {
            m_font->setBaseSize((int)val[0]);
            });

        int num = m_numGlyph;
        int start = 0, end = 0;
        m_CurrentPageIndex = pageIndex;
        if (num <= LIMIT_GLYPH_PER_PAGE)
        {
            start = 0;
            end = num;
            m_CurrentPageIndex = 0;
            m_MaxPageIndex = 0;
        }
        else
        { 
            start = m_CurrentPageIndex * LIMIT_GLYPH_PER_PAGE;
            m_MaxPageIndex =  (num - (num % LIMIT_GLYPH_PER_PAGE)) / LIMIT_GLYPH_PER_PAGE;
            m_MaxPageIndex += (num % LIMIT_GLYPH_PER_PAGE) > 0 ? 1 : 0;

            if (start >= num)
            {
                start = num - LIMIT_GLYPH_PER_PAGE;
                end = num;
            }
            else
            {
                end = start + LIMIT_GLYPH_PER_PAGE;
                if (end > num) end = num;
            }
        }

        m_StartGlyph = start;
        m_EndGlyph = end;

        for (int i = 0; i < LIMIT_GLYPH_PER_PAGE; i++)
        {
            std::shared_ptr<BitmapGlyphEditor> widget = nullptr;
            if (i < m_glyphList.size())
            {
                widget = m_glyphList[i];
            }
            else
            {
                widget = std::make_shared<BitmapGlyphEditor>();
                m_glyphList.push_back(widget);
            }
            
            if (m_StartGlyph + i < m_EndGlyph)
                widget->Draw(m_font->getGlyph(m_StartGlyph + i), m_fontPageGroup);
            else
                widget->Clear();
        }

        auto bottomGroup = m_fontPageGroup->createWidget<Group>("G1", false, false);
        auto btnPrev = bottomGroup->createWidget<Button>("Prev Page", ImVec2(128.f, 0.f));
        btnPrev->setEndOfLine(false);
        btnPrev->getOnClickEvent().addListener([this](auto widget) {
            m_bPageFlag = true;
            if(m_CurrentPageIndex > 0)
                m_CurrentPageIndex--;
            });

        auto btnNext = bottomGroup->createWidget<Button>("Next Page", ImVec2(128.f, 0.f));
        btnNext->getOnClickEvent().addListener([this](auto widget) {
            m_bPageFlag = true;
            if(m_MaxPageIndex > m_CurrentPageIndex)
                m_CurrentPageIndex++;
            });
        btnNext->setEndOfLine(true);

        m_bPageFlag = false;
    }

    void BitmapFontCreator::_drawTexture()
    {
        if (m_columns == nullptr || m_GroupRight == nullptr || !m_bTextureFlag) return;
        if (m_texture == nullptr) return;
        
        if (m_imgWidget) {
            m_imgWidget->removeAllPlugins();
            m_imgWidget = nullptr;
        }

        m_columns->setColumnWidth(1, m_texture->GetTextureWidth() + 10.0f);
        ImVec2 size(m_texture->GetTextureWidth(), m_texture->GetTextureHeight());
        m_imgWidget = m_GroupRight->createWidget<Image>(m_texture->GetTextureHandle(), size);

        m_bTextureFlag = false;
    }

    void BitmapFontCreator::dirty()
    {
        m_bPanelFlag = true;
        m_bPageFlag = true;
    }

    void BitmapFontCreator::onLoadImg(const std::string& path)
    {
        if (m_imgWidget) {
            m_imgWidget->removeAllPlugins();
            m_imgWidget = nullptr;
        }

        if (m_texture != nullptr) {
            m_texture->DecReference();
            m_texture = nullptr;
        }


        if (!path.empty()) {
            auto fsPath = fs::path(path);
            auto relPath = fsPath.is_absolute() ? fs::relative(fs::path(path), fs::current_path()).string() : fsPath.string();
            std::replace(relPath.begin(), relPath.end(), '\\', '/');
            m_path = relPath;
            m_texture = ResourceCreator::Instance().NewTexture(m_path.c_str());
            m_texture->WaitInitialize();
            m_texture->WaitBuild();
        }
        else
            m_path = path;

        m_bTextureFlag = true;

        if (m_font != nullptr) m_font->setName(m_path);
    }

    void BitmapFontCreator::setChacterCode(std::string& characterSet)
    {
        m_CharacterCode = characterSet;
    }

    std::string BitmapFontCreator::getChacterCode() const
    {
        std::string s = m_CharacterCode;
        s = removeSpaces(s);
        s += ' ';
        return s;
    }

    void BitmapFontCreator::generateCode(const std::string& code)
    {
        int len = code.length();
        if (len == 0) return;
        m_font = std::make_shared<BitmapFont>(m_path);
        m_numGlyph = len;
        wchar_t dest[512];
        int lenW =  mbstowcs(dest, code.c_str(), 512);
        if (dest != nullptr)
        {
            for (int i = 0; i < lenW; i++)
            {
                auto glyph = std::make_shared<BitmapGlyph>();
                glyph->ID = i;
                glyph->Unicode = dest[i];
                glyph->Name = "";
                m_font->addGlyph(glyph);
            }
            m_bPageFlag = true;
            m_CurrentPageIndex = 0;
        }
    }

    bool BitmapFontCreator::saveBitmapFont()
    {
        if (m_font == nullptr || m_path.empty()) return false;
        json jObj;
        m_font->To_json(jObj);

        auto fsPath = fs::path(m_path);
        auto ext = fsPath.extension();
        if (ext.string() != ".pybm")
        {
            fsPath = fsPath.replace_extension(".pybm");
        }
        std::ofstream file(fsPath.string());
        file << jObj;

        return true;
    }

    bool BitmapFontCreator::loadBitmapFont(const std::string& path)
    {
        if (path.empty()) return false;
        
        if (m_font != nullptr) m_font = nullptr;
        m_font = std::make_shared<BitmapFont>("");
        BitmapFontHelper::getInstance()->loadBitmapFromPath(path, m_font);
        if (m_font != nullptr) {
            m_numGlyph = m_font->getGlyphCount();
            m_path = m_font->getName();
            m_CurrentPageIndex = 0;
            return true;
        }
        return false;
    }
}
