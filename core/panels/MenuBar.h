#pragma once

#include <unordered_map>

#include "core/Panel.h"

namespace ige::creator
{
    class MenuItem;

    class MenuBar: public Panel
    {
    public:
        MenuBar(const std::string& name = "", const Panel::Settings& settings = {});
        virtual ~MenuBar();

        virtual void initialize();

    protected:
        virtual void _drawImpl() override;
    
    private:
        void createFileMenu();

    private:
        std::unordered_map<std::string, std::pair<std::shared_ptr<Panel>, std::shared_ptr<MenuItem>>> m_panels;
        
    };
}