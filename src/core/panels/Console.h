#pragma once
#include <deque>

#include "core/Panel.h"
#include "core/layout/Group.h"

namespace ige::creator
{
    class Console: public Panel
    {
    public:
        Console(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~Console();

        void onLogged(const char* message);
        void clearAllLogs();
        
        //! Auto clear on start
        bool isAutoClearOnStart() { return m_bAutoClearOnStart; }
        void setAutoClearOnStart(bool autoClear = true) { m_bAutoClearOnStart = autoClear; }
        
        //! Auto scroll
        bool isAutoScroll() { return m_bAutoScroll; }
        void setAutoScroll(bool autoScroll = true) { m_bAutoScroll = autoScroll; }

        //! Max number of lines
        int getMaxLines() const { return m_maxLines; }
        void setMaxLines(int max) { m_maxLines = max; }

    protected:
        virtual void initialize() override;
        virtual void clear();
        virtual void _drawImpl() override;

    protected:
        std::shared_ptr<Group> m_topGroup = nullptr;
        std::shared_ptr<Group> m_logGroup = nullptr;
        std::deque<uint64_t> m_logIds;

        int m_maxLines = 3000;
        int _scrollToBottom = 0;
        bool m_bAutoClearOnStart = true;
        bool m_bAutoScroll = true;
    };
}
