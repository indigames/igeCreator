#pragma once
#include <sstream>

#include "core/Panel.h"
#include "core/layout/Group.h"
#include "core/widgets/TextArea.h"

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
        void setAutoScroll(bool autoScroll = true);

        //! Max number of lines
        int getMaxLogSize() const { return m_maxLogSize; }
        void setMaxLogSize(int max) { m_maxLogSize = max; }

    protected:
        virtual void initialize() override;
        virtual void clear();
        virtual void _drawImpl() override;

    protected:
        std::shared_ptr<Group> m_topGroup = nullptr;
        std::shared_ptr<Group> m_logGroup = nullptr;
        std::shared_ptr<TextArea> m_logTextWidget = nullptr;
        std::string m_logBuffer;

        int m_maxLogSize = 4096;
        bool m_bAutoClearOnStart = true;
        bool m_bAutoScroll = true;
    };
}
