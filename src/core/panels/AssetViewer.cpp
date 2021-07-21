#include <imgui.h>

#include "core/panels/AssetViewer.h"
#include "core/panels/AssetBrowser.h"

#include "core/Editor.h"
#include "core/Canvas.h"

#include "core/widgets/Label.h"
#include "core/widgets/CheckBox.h"
#include "core/widgets/TextField.h"
#include "core/widgets/Drag.h"

namespace ige::creator
{
    AssetViewer::AssetViewer(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        close();
    }
    
    AssetViewer::~AssetViewer()
    {

    }

    void AssetViewer::initialize()
    {
        clear();
    }

    void AssetViewer::_drawImpl()
    {
        auto path = Editor::getCanvas()->getAssetBrowser()->getSelectedPath();
        if (!path.empty() || path.compare("") != 0 && m_path.compare(path) != 0)
        {
            m_path = path;
            removeAllWidgets();

            auto fsPath = fs::path(m_path);
            auto metaPath = fsPath.parent_path().append(fsPath.filename().string() + ".meta");
            std::ifstream file(metaPath);
            if (file.is_open())
            {
                json metaJs;
                file >> metaJs;
                file.close();
                
                for (auto& [key, val] : metaJs.items())
                {
                    if(val.type() == nlohmann::json::value_t::boolean)
                        createWidget<CheckBox>(key, val);
                    else if (val.type() == nlohmann::json::value_t::number_float)
                        createWidget<Drag<float>>(key, ImGuiDataType_Float, val);
                    else if(val.type() == nlohmann::json::value_t::number_integer)
                        createWidget<Drag<int32_t>>(key, ImGuiDataType_S32, val);
                    else if(val.type() == nlohmann::json::value_t::number_unsigned)
                        createWidget<Drag<uint32_t>>(key, ImGuiDataType_U32, val);
                    else if (val.type() == nlohmann::json::value_t::string)
                        createWidget<TextField>(key, val);
                    else {
                        // Todo: parse Vec<N>, Quat, ...
                    }
                }
                open();
            }
            else
            {
                close();
            }
        }
        Panel::_drawImpl();
    }

    void AssetViewer::clear()
    {

    }
}
