#include <imgui.h>

#include "core/panels/AssetBrowser.h"

#include "core/widgets/Button.h"
#include "core/widgets/TreeNode.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Icon.h"
#include "core/layout/Group.h"
#include "core/plugin/DDSourcePlugin.h"

#include "core/dialog/MsgBox.h"

namespace ige::creator
{
    AssetBrowser::AssetBrowser(const std::string& name, const Panel::Settings& settings, const std::string& engineAssetPath, const std::string& projectAssetPath)
        : Panel(name, settings), m_engineAssetFolder(engineAssetPath), m_projectAssetFolder(projectAssetPath)
    {
        // Create project assets folder if it was not found
        if (!std::filesystem::exists(projectAssetPath))
        {
            std::filesystem::create_directories(projectAssetPath);
        }
                
        createWidget<Button>("Refresh")->getOnClickEvent().addListener(std::bind(&AssetBrowser::refresh, this));
        createWidget<Button>("Import")->getOnClickEvent().addListener(std::bind(&AssetBrowser::import, this, m_projectAssetFolder));

        m_assetGroup = createWidget<Group>("AssetGroup");

        refresh();
    }
    
    AssetBrowser::~AssetBrowser()
    {
        clear();
        m_assetGroup = nullptr;
    }

    void AssetBrowser::initialize()
    {
        clear();
    }

    void AssetBrowser::_drawImpl()
    {
        Panel::_drawImpl();
    }

    void AssetBrowser::clear()
    {
        m_assetGroup->removeAllWidgets();
        m_pathUpdate.clear();
    }


    void AssetBrowser::refresh()
    {
        clear();

        if (m_engineAssetFolder == "")
        {
            considerItem(nullptr, fs::directory_entry(fs::current_path()), true);
        }
        else
        {
            considerItem(nullptr, fs::directory_entry(m_engineAssetFolder), true);
        }
        m_assetGroup->createWidget<Separator>();

        if (m_projectAssetFolder != "")
        {
            considerItem(nullptr, fs::directory_entry(m_projectAssetFolder), true);
            m_assetGroup->createWidget<Separator>();
        }
    }


    void AssetBrowser::import(const std::string& path)
    {

    }

    void AssetBrowser::parseFolder(const std::shared_ptr<TreeNode>& root, const fs::directory_entry& directory, bool isEngine)
    {
        /* Iterates folders */
        for (auto& item : fs::directory_iterator(directory))
            if (item.is_directory())
                considerItem(root, item, isEngine);

        /* Iterates files */
        for (auto& item : fs::directory_iterator(directory))
            if (!item.is_directory())
                considerItem(root, item, isEngine);
    }

    void AssetBrowser::considerItem(const std::shared_ptr<TreeNode>& root, const fs::directory_entry& entry, bool isEngine)
    {
        if(!entry.exists()) return;
        
        std::string itemName = entry.path().filename().string();
        std::string path = entry.path().string();

        if (entry.is_directory())
        {
            auto treeNode = root ? root->createWidget<TreeNode>(itemName) : m_assetGroup->createWidget<TreeNode>(itemName);
            treeNode->getOnOpenedEvent().addListener([&, this, treeNode, path](){
                parseFolder(treeNode, std::filesystem::directory_entry(path), isEngine);
            });
        }
        else
        {
            auto fileIcon = root ? root->createWidget<Icon>(itemName, 0) : m_assetGroup->createWidget<Icon>(itemName, 0);
            fileIcon->addPlugin<DDSourcePlugin<std::string>>(fileIcon->getId(), fileIcon->getLabel(), path);
        }
    }
}
