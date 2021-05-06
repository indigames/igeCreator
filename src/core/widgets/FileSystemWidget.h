#pragma once

#include <imgui.h>
#include <utils/PyxieHeaders.h>

#include "core/Widget.h"
#include "core/filesystem/FileSystem.h"
#include "core/filesystem/FileSystemCache.h"

namespace ige::creator
{
    class FileSystemWidget : public Widget
    {
    public:
        FileSystemWidget();
        FileSystemWidget(const fs::directory_cache& cache);
        virtual ~FileSystemWidget();
        void setDirty() { m_isDirty = true; }

    protected:
        virtual void _drawImpl() override;
        
    private:
        fs::directory_cache m_cache;
        fs::path m_root;
        fs::path m_selection;        
        bool m_isDirty;
        std::vector<fs::path> m_hierarchy;
        std::unordered_map<std::string, pyxie::pyxieTexture*> m_iconTextures;
        
        bool ProcessDragDropSource(pyxieTexture* preview, const fs::path& absolute_path);
        void ProcessDragDropTarget(const fs::path& absolute_path);
        void SetCachePath(const fs::path& path);
        bool DrawEntry(pyxieTexture* icon, bool is_loading, const std::string& name,
            const fs::path& absolute_path, bool is_selected, const float size,
            const std::function<void()>& on_click, const std::function<void()>& on_double_click,
            const std::function<void(const std::string&)>& on_rename,
            const std::function<void()>& on_delete);
    };
}
