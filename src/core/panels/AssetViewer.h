#pragma once

#include "core/Panel.h"
#include "core/layout/Group.h"
#include "core/scene/assets/AssetMeta.h"

namespace ige::creator
{
    class AssetViewer: public Panel
    {
    public:
        AssetViewer(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~AssetViewer();

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

    protected:
        std::string m_path = "";
        std::shared_ptr<Group> m_assetGroup = nullptr;
        std::shared_ptr<AssetMeta> m_assetMeta = nullptr;
    };
}
