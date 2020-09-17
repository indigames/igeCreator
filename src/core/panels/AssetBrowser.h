#pragma once

#include <unordered_map>

#include "core/Panel.h"
#include "core/layout/Columns.h"

#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

#include "pyxieTexture.h"
using namespace pyxie;

namespace ige::creator
{
    class Group;
    class TreeNode;
    class AssetBrowser: public Panel
    {
    public:
        AssetBrowser(const std::string& name = "", const Panel::Settings& settings = {}, const std::string& engineAssetPath = "", const std::string& projectAssetPath = "");
        virtual ~AssetBrowser();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;
        
        std::string m_engineAssetFolder;
        std::string m_projectAssetFolder;
        std::string m_currentFolder;
    };
}
