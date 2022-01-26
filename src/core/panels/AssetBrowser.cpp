#include "core/panels/AssetBrowser.h"
#include "core/widgets/Button.h"
#include "core/widgets/TreeNode.h"
#include "core/widgets/Separator.h"
#include "core/widgets/Icon.h"
#include "core/widgets/Label.h"
#include "core/widgets/FileSystemWidget.h"
#include "core/FileHandle.h"
#include "core/layout/Group.h"
#include "core/layout/Columns.h"
#include "core/plugin/DragDropPlugin.h"

#include "core/dialog/MsgBox.h"
#include "core/dialog/OpenFileDialog.h"

#include "core/Editor.h"
#include "core/scene/assets/FigureMeta.h"
#include "core/scene/assets/TextureMeta.h"

#include "core/utils/crc32.h"

#include <utils/GraphicsHelper.h>
#include <SDL.h>
#include <imgui.h>
#include <string>
#include <algorithm>

namespace ige::creator
{
    fs::recursive_directory_cache g_cache;
    std::thread g_watcherThread;
    std::mutex g_watcherMutex;
    bool g_stopWatcherThread = false;
    bool g_assetMetaChecked = false;

    void watchThreadFunc()
    {
        g_cache.set_path(Editor::getInstance()->getProjectPath());
        g_cache.set_scan_frequency(std::chrono::milliseconds(1000));

        while (true) {
            // lock scope
            {
                std::unique_lock<std::mutex> lk(g_watcherMutex);
                if (g_stopWatcherThread)
                {
                    lk.unlock();
                    return;
                }
                lk.unlock();
            }

            auto size = g_cache.size();
            if (!g_cache.isProcessed() && size > 0) {
                for (const auto& file : g_cache) {
                    const auto& fsPath = file.entry.path();
                    const auto& name = file.stem;
                    const auto& relative = file.protocol_path;
                    const auto& file_ext = file.extension;
                    const std::string& filename = file.filename;

                    if (IsFormat(E_FileExts::Hidden, file_ext))
                        continue;

                    auto hidden = false;
                    auto path = fsPath.string();
                    std::transform(path.begin(), path.end(), path.begin(), ::tolower);
                    const auto& hiddenItems = GetFileExtensionSuported(E_FileExts::Hidden);
                    for (const auto& item : hiddenItems) {
                        if (path.find(item) != std::string::npos) {
                            hidden = true;
                            break;
                        }
                    }
                    if (hidden) continue;
                    if (fs::is_regular_file(file.entry.status()) && file_ext.compare(".meta") != 0) {
                        const auto metaPath = fsPath.parent_path().append(filename + ".meta");

                        // check file exist
                        bool dirty = !fs::exists(metaPath);

                        // check crc
                        if (!dirty) {
                            auto file = std::ifstream(metaPath);
                            if (file.is_open()) {
                                try {
                                    json metaJs;
                                    file >> metaJs;
                                    file.close();
                                    auto crc = metaJs.value("CRC", (uint32_t)-1);
                                    try {
                                        auto newCrc = crc32::crc32_from_file(fsPath.string().c_str());
                                        if (newCrc != crc) {
                                            dirty = true;
                                        }
                                    }
                                    catch (std::exception e) {
                                    }
                                }
                                catch (std::exception e) {
                                    dirty = true;
                                }
                            }
                        }

                        // check figure dirty
                        if (!dirty) {
                            if (IsFormat(E_FileExts::Figure, file_ext)) {
                                auto figureMeta = std::make_unique<FigureMeta>(path);
                                if(figureMeta->isAnim()) {
                                    if (!fs::exists(fsPath.parent_path().append(fsPath.stem().string() + ".pyxa"))) {
                                        dirty = true;
                                    }
                                }
                                else if (figureMeta->isBaseModel()) {
                                    auto basePath = fs::path(figureMeta->baseModelPath());
                                    if (!fs::exists(basePath.parent_path().append(basePath.stem().string() + ".pyxf"))) {
                                        dirty = true;
                                    }
                                }
                            }
                        }

                        // resave
                        if (dirty) {
                            if (IsFormat(E_FileExts::Figure, file_ext)) {
                                std::make_unique<FigureMeta>(path)->save();
                            }
                            else if (IsFormat(E_FileExts::Sprite, file_ext)) {
                                std::make_unique<TextureMeta>(path)->save();
                            }
                            else {
                                std::make_unique<AssetMeta>(path)->save();
                            }
                        }
                    }
                }
                g_cache.setProcessed(true);
            }

            if(!g_assetMetaChecked)
                g_assetMetaChecked = true;

            // sleep 1s
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void stopWatcherThread() {
        g_assetMetaChecked = false;
        std::unique_lock<std::mutex> lk(g_watcherMutex);
        g_stopWatcherThread = true;
        lk.unlock();
        if (g_watcherThread.joinable())
            g_watcherThread.join();
    }

    void startWatcherThread() {
        stopWatcherThread();
        std::unique_lock<std::mutex> lk(g_watcherMutex);
        g_stopWatcherThread = false;
        lk.unlock();
        g_watcherThread = std::thread(watchThreadFunc);
    }

    AssetBrowser::AssetBrowser(const std::string& name, const Panel::Settings& settings)
        : Panel(name, settings)
    {
        m_bLastFocused = false;
    }
    
    AssetBrowser::~AssetBrowser()
    {
        stopWatcherThread();
        m_fileSystemWidget = nullptr;
        removeAllWidgets();
        getOnFocusEvent().removeAllListeners();
    }

    void AssetBrowser::initialize()
    {
        m_fileSystemWidget = createWidget<FileSystemWidget>();
        for (const auto& widget : m_widgets) {
            if (widget != nullptr) {
                widget.get()->getOnClickEvent().addListener([this](Widget*) {
                    m_focusEvent.invoke();
                });
            }
        }
    }

    void AssetBrowser::setDirty()
    {
        startWatcherThread();
        std::static_pointer_cast<FileSystemWidget>(m_fileSystemWidget)->setDirty();

        // Wait until meta data was checked sucessfully the first time
        while (!g_assetMetaChecked) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
  
    std::string AssetBrowser::getSelectedPath()
    {
        return std::static_pointer_cast<FileSystemWidget>(m_fileSystemWidget)->getSelectedPath();
    }
}
