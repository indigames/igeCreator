#include "core/CommandManager.h"
#include "scene/SceneManager.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"

#include "core/Editor.h"

NS_IGE_BEGIN

CommandManager::CommandManager() 
{

}

CommandManager::~CommandManager()
{
    
}

void CommandManager::PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->uuid = target->getUUID();
    cmd->uuid_parent = target->getParent() != nullptr ? target->getParent()->getUUID() : ""; 
    json jObj;
    target->to_json(jObj);
    cmd->jObj = jObj;
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}

void CommandManager::PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target, json& jObj)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->uuid = target->getUUID();
    cmd->uuid_parent = target->getParent() != nullptr ? target->getParent()->getUUID() : "";
    cmd->jObj = jObj;
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}

void CommandManager::Undo()
{
    if (m_undo.size() == 0) return;
    auto item = m_undo.back();
    if (item->type == COMMAND_TYPE::EDIT_COMPONENT) {
        auto newItem = generateComponent(item);
        m_redo.push_back(newItem);
    }
    else {
        m_redo.push_back(item);
    } 
    m_undo.pop_back();
    activeUndoCommand(item);
}

void CommandManager::Redo()
{
    if (m_redo.size() == 0) return;
    auto item = m_redo.back();
    if (item->type == COMMAND_TYPE::EDIT_COMPONENT) {
        auto newItem = generateComponent(item);
        m_undo.push_back(newItem);
    }
    else {
        m_undo.push_back(item);
    }
    m_redo.pop_back();
    activeRedoCommand(item);
}

std::shared_ptr<Command> CommandManager::generateComponent(std::shared_ptr<Command> command)
{
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    auto scene = SceneManager::getInstance()->getCurrentScene();
    auto target = scene->findObjectByUUID(command->uuid);
    json jObj;
    target->to_json(jObj);

    cmd->uuid = command->uuid;
    cmd->uuid_parent = command->uuid_parent;
    cmd->jObj = jObj;
    cmd->type = command->type;  
    return cmd;
}


void CommandManager::activeUndoCommand(std::shared_ptr<Command> command)
{
    auto scene = SceneManager::getInstance()->getCurrentScene();
    auto target = scene->findObjectByUUID(command->uuid);
    auto parent = scene->findObjectByUUID(command->uuid_parent);
    
    auto type = command->type;
    switch (type) {
    case COMMAND_TYPE::ADD_OBJECT:
    {
        if (target != nullptr)
        {
            Editor::getInstance()->removeTarget(target);
            Editor::getCurrentScene()->removeObject(target);
        }
    }
    break;
    case COMMAND_TYPE::DELETE_OBJECT:
    {
        auto newObject = scene->createObject(command->jObj.value("name", std::string()), nullptr, command->jObj.value("gui", false));
        newObject->from_json(command->jObj);
        newObject->setParent(parent);
    }
    break;
    case COMMAND_TYPE::SELECT_OBJECT:
        break;
    case COMMAND_TYPE::ADD_COMPONENT:
    {
        if (target != nullptr)
        {
            auto j = command->jObj;
            auto jComps = j.at("comps");
            for (auto it : jComps)
            {
                std::string key = it.at(0);
                target->removeComponent(key);
            }
        }
    }
    break;
    case COMMAND_TYPE::DELETE_COMPONENT:
    {
        if (target != nullptr)
        {
            auto j = command->jObj;
            auto jComps = j.at("comps");
            for (auto it : jComps)
            {
                std::string key = it.at(0);
                auto val = it.at(1);
                std::shared_ptr<Component> comp = target->getComponent(key);
                if (comp == nullptr)
                    comp = target->createComponent(key);
                if (comp) {
                    val.get_to(*comp);
                }
            }
        }
    }
    break;
    case COMMAND_TYPE::EDIT_COMPONENT:
    {
        if (target != nullptr)
        {
            auto j = command->jObj;
            auto jComps = j.at("comps");
            for (auto it : jComps)
            {
                std::string key = it.at(0);
                auto val = it.at(1);
                std::shared_ptr<Component> comp = target->getComponent(key);
                if (comp == nullptr)
                    comp = target->createComponent(key);
                if (comp) {
                    val.get_to(*comp);
                }
            }
        }
    }
    break;
    case COMMAND_TYPE::NONE:
    default:
        break;
    }
    
}

void CommandManager::activeRedoCommand(std::shared_ptr<Command> command)
{
    auto scene = SceneManager::getInstance()->getCurrentScene();
    auto target = scene->findObjectByUUID(command->uuid);
    auto parent = scene->findObjectByUUID(command->uuid_parent);

    auto type = command->type;
    switch (type) {
    case COMMAND_TYPE::ADD_OBJECT:
    {
        auto newObject = scene->createObject(command->jObj.value("name", std::string()), nullptr, command->jObj.value("gui", false));
        newObject->from_json(command->jObj);
        newObject->setParent(parent);
    }
    break;
    case COMMAND_TYPE::DELETE_OBJECT:
    {
        if (target != nullptr)
        {
            Editor::getInstance()->removeTarget(target);
            Editor::getCurrentScene()->removeObject(target);
        }
    }
    break;
    case COMMAND_TYPE::SELECT_OBJECT:
        break;
    case COMMAND_TYPE::ADD_COMPONENT:
    {
        if (target != nullptr)
        {
            auto j = command->jObj;
            auto jComps = j.at("comps");
            for (auto it : jComps)
            {
                std::string key = it.at(0);
                auto val = it.at(1);
                std::shared_ptr<Component> comp = target->getComponent(key);
                if (comp == nullptr)
                    comp = target->createComponent(key);
                if (comp) {
                    val.get_to(*comp);
                }
            }
        }
    }
    break;
    case COMMAND_TYPE::DELETE_COMPONENT:
    {
        if (target != nullptr)
        {
            auto j = command->jObj;
            auto jComps = j.at("comps");
            for (auto it : jComps)
            {
                std::string key = it.at(0);
                target->removeComponent(key);
            }
        }
    }
    break;
    case COMMAND_TYPE::EDIT_COMPONENT:
    {
        auto j = command->jObj;
        auto jComps = j.at("comps");
        for (auto it : jComps)
        {
            std::string key = it.at(0);
            auto val = it.at(1);
            std::shared_ptr<Component> comp = target->getComponent(key);
            if (comp == nullptr)
                comp = target->createComponent(key);
            if (comp) {
                val.get_to(*comp);
            }
        }
    }
    break;
    case COMMAND_TYPE::NONE:
    default:
        break;
    }

}


NS_IGE_END