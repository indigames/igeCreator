#pragma once


#include "core/Panel.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

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
        std::string getSelectedPath();

    protected:
        virtual void initialize() override;

        bool m_bLastFocused = false;
        ige::scene::Event<> m_focusEvent;
        std::shared_ptr<Widget> m_fileSystemWidget = nullptr;
    };
}
