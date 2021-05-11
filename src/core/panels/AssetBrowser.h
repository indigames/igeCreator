#pragma once

#include <unordered_map>

#include "core/Panel.h"
#include "core/layout/Columns.h"

#include "pyxieTexture.h"
using namespace pyxie;

namespace ige::creator
{
    class Group;
    class TreeNode;
    class AssetBrowser: public Panel
    {
    public:
        AssetBrowser(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AssetBrowser();
        ige::scene::Event<>& getOnFocusEvent() { return m_focusEvent; }
        void setDirty();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        bool m_bLastFocused = false;
        ige::scene::Event<> m_focusEvent;
        std::shared_ptr<Widget> m_fileSystemWidget = nullptr;
    };
}
