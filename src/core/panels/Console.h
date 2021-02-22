#pragma once

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

        virtual void clear();

    protected:
        virtual void initialize() override;
        virtual void _drawImpl() override;

    protected:
        std::shared_ptr<Group> m_logGroup = nullptr;

        int _scrollToBottom = 0;
    };
}
