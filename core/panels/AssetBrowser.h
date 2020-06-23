#pragma once

#include <unordered_map>

#include <filesystem>
namespace fs = std::filesystem;

#include "core/Panel.h"
#include "core/layout/Columns.h"

namespace ige::creator
{
    class Group;
    class TreeNode;
    class AssetBrowser: public Panel
    {
    public:
        AssetBrowser(const std::string& name = "", const Panel::Settings& settings = {}, const std::string& engineAssetPath = "", const std::string& projectAssetPath = "");
        virtual ~AssetBrowser();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

        void parseFolder(const std::shared_ptr<TreeNode>& root, const fs::directory_entry& directory, bool isEngine = false);
        void parseFile(const fs::directory_entry& directory, bool isEngine = false);

        void refresh();
        void import(const std::string& path);

        std::string m_engineAssetFolder;
        std::string m_projectAssetFolder;        
        std::string m_currentFolder;
        std::shared_ptr<Group> m_folderGroup;
        std::shared_ptr<Columns<4>> m_fileGroup;        
        std::shared_ptr<TreeNode> m_selectedNode;
    };
}
