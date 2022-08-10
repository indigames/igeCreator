#pragma once

#include "utils/PyxieHeaders.h"
#include "core/Macros.h"

#include "core/Panel.h"
#include "core/layout/Group.h"
#include <core/layout/Columns.h>

#include "core/BitmapFontHelper.h"
#include "core/BitmapFont.h"

USING_NS_IGE_SCENE

namespace ige::creator
{
    class Image;
    class TextArea;

    class BitmapGlyphEditor
    {
    public:
        BitmapGlyphEditor();
        ~BitmapGlyphEditor();

        void Clear();
        void Draw(std::shared_ptr<BitmapGlyph> data, std::shared_ptr<Group> parent);

        std::shared_ptr<BitmapGlyph> getData() { return m_data; }
        void setData(std::shared_ptr<BitmapGlyph>  data);

    protected:
        std::shared_ptr<BitmapGlyph> m_data;
        std::shared_ptr<Group> m_glyphGroup = nullptr;
    };

    class BitmapFontCreator : public Panel
    {
    public:
        BitmapFontCreator(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~BitmapFontCreator();

        virtual void clear();
        virtual void update(float dt) override;

        void setChacterCode(const std::string &characterSet);
        bool saveBitmapFont();
        bool loadBitmapFont(const std::string& path);

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        void _drawPanel();
        void _drawPage(int pageIndex);
        void _drawTexture();

        void dirty();
        
        void onLoadImg(const std::string& path);

        std::string getChacterCode() const;
        void generateCode(const std::string& code);

    protected:
        std::shared_ptr<Group> m_topGroup = nullptr;
        std::shared_ptr<Group> m_fontGroup = nullptr;
        std::shared_ptr<Group> m_fontPageGroup = nullptr;
        std::shared_ptr<Columns<2>> m_columns = nullptr;
        std::shared_ptr<Group> m_GroupLeft = nullptr;
        std::shared_ptr<Group> m_GroupRight = nullptr;
        std::shared_ptr<Image> m_imgWidget = nullptr;
        std::shared_ptr<TextArea> m_txtChars = nullptr;

        std::vector<std::shared_ptr<BitmapGlyphEditor>> m_glyphList;

        bool m_bPanelFlag = false;
        bool m_bPageFlag = false;
        bool m_bTextureFlag = false;

        int m_CurrentPageIndex = 0;
        int m_MaxPageIndex = 0;
        int m_StartGlyph = 0;
        int m_EndGlyph = 0;

        std::string m_path;
        std::string m_CharacterCode;
        Texture* m_texture = nullptr;
        std::shared_ptr<BitmapFont> m_font = nullptr;
        int m_numGlyph = 0;
    };
}
