#include <imgui.h>

#include "core/panels/AssetBrowser.h"

#include "core/widgets/Button.h"
#include "core/widgets/TreeNode.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Icon.h"
#include "core/layout/Group.h"
#include "core/layout/Columns.h"
#include "core/plugin/DragDropPlugin.h"

#include "core/dialog/MsgBox.h"
#include "core/dialog/OpenFileDialog.h"

namespace ige::creator
{
    AssetBrowser::AssetBrowser(const std::string& name, const Panel::Settings& settings, const std::string& engineAssetPath, const std::string& projectAssetPath)
        : Panel(name, settings), m_engineAssetFolder(engineAssetPath), m_projectAssetFolder(projectAssetPath)
    {
        m_selectedNode = nullptr;
    }
    
    AssetBrowser::~AssetBrowser()
    {
        clear();
        m_folderGroup = nullptr;
        m_fileGroup = nullptr;
        m_selectedNode = nullptr;
        removeAllWidgets();
    }

    void AssetBrowser::initialize()
    {
        // Create project assets folder if it was not found
        if (!std::filesystem::exists(m_projectAssetFolder))
        {
            std::filesystem::create_directories(m_projectAssetFolder);
        }

        createWidget<Button>("Refresh", ImVec2(0, 0), true, false)->getOnClickEvent().addListener([this]() {
            m_bNeedRefresh = true;
        });
        createWidget<Button>("Import", ImVec2(0, 0))->getOnClickEvent().addListener(std::bind(&AssetBrowser::import, this, m_projectAssetFolder));

        auto columns = createWidget<Columns<2>>();
        columns->setColumnWidth(0, 180.0f);
        columns->setColumnWidth(1, 680.f);

        m_folderGroup = columns->createWidget<Group>("FolderGroup", false);
        m_fileGroup = columns->createWidget<Columns<4>>(180.f, true);

        refresh();
    }

    void AssetBrowser::_drawImpl()
    {
        Panel::_drawImpl();

        if (m_bNeedRefresh)
        {
            refresh();
            m_bNeedRefresh = false;
        }
    }

    void AssetBrowser::clear()
    {
        m_folderGroup->removeAllWidgets();
        m_fileGroup->removeAllWidgets();
    }


    void AssetBrowser::refresh()
    {
        clear();
        m_currentFolder = m_projectAssetFolder != "" ? m_projectAssetFolder : fs::current_path().string();

        auto rootDir = fs::directory_entry(m_currentFolder);
        std::string itemName = rootDir.path().filename().string();
        std::string path = rootDir.path().string();

        auto treeNode = m_folderGroup->createWidget<TreeNode>(itemName, false, false, true);
        treeNode->getOnClickEvent().addListener([&, treeNode, this, path]() {
            m_currentFolder = path;
            parseFile(std::filesystem::directory_entry(path), true);
            if (m_selectedNode != treeNode)
            {
                m_selectedNode->setIsSelected(false);
                m_selectedNode = treeNode;
                m_selectedNode->setIsSelected(true);
            }
        });
        m_selectedNode = treeNode;
        m_selectedNode->setIsSelected(true);

        if (rootDir.exists() && rootDir.is_directory())
        {
            parseFolder(treeNode, rootDir, true);
        }
    }


    void AssetBrowser::import(const std::string& path)
    {
        auto files = OpenFileDialog("Import Assets", m_currentFolder, { "All Files", "*" }, OpenFileDialog::Option::multiselect).result();
        for (auto file : files)
        {
            auto fname = fs::path(file).filename().string();
            if (fs::exists(fs::path(m_currentFolder).append(fname)))
            {  
                auto content = "File " + fname + " exists. Do you want to replace?";
                auto overwrite = MsgBox("File exists", content, MsgBox::EBtnLayout::yes_no, MsgBox::EMsgType::question).result();
                if (overwrite == MsgBox::EButton::yes)
                {
                    fs::copy(file, m_currentFolder, fs::copy_options::overwrite_existing);
                }
            }
            else
            {
                fs::copy(file, m_currentFolder, fs::copy_options::none);
            }            
        }
    }

    void AssetBrowser::parseFolder(const std::shared_ptr<TreeNode>& root, const fs::directory_entry& directory, bool isEngine)
    {
        if(!directory.exists()) return;
        
        for (auto& item : fs::directory_iterator(directory))
        {
            if (item.is_directory())
            {
                std::string itemName = item.path().filename().string();
                std::string path = item.path().string();

                auto treeNode = root ? root->createWidget<TreeNode>(itemName) : m_folderGroup->createWidget<TreeNode>(itemName);
                treeNode->getOnClickEvent().addListener([&, treeNode, this, path]() {
                    m_currentFolder = path;
                    parseFile(std::filesystem::directory_entry(path), isEngine);
                    if (m_selectedNode != treeNode)
                    {
                        m_selectedNode->setIsSelected(false);
                        m_selectedNode = treeNode;
                        m_selectedNode->setIsSelected(true);
                    }
                });
                parseFolder(treeNode, item, isEngine);
            }
        }
    }
    
    void AssetBrowser::parseFile(const fs::directory_entry& directory, bool isEngine)
    {
        m_fileGroup->removeAllWidgets();

        for (auto& item : fs::directory_iterator(directory))
        {
            if (!item.is_directory())
            {
                std::string itemName = item.path().filename().string();
                std::string path = item.path().string();

                auto fileIcon = m_fileGroup->createWidget<Icon>(itemName, 0, ImVec2(32.f, 32.f));
                fileIcon->addPlugin<DDSourcePlugin<std::string>>(EDragDropID::FILE, fileIcon->getLabel(), path);
            }
        }
    }
}
