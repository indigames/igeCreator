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

// SceneObject
void CommandManager::PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->objType = 0;
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
    cmd->objType = 0;
    cmd->uuid = target->getUUID();
    cmd->uuid_parent = target->getParent() != nullptr ? target->getParent()->getUUID() : "";
    cmd->jObj = jObj;
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}

void CommandManager::PushCommand(COMMAND_TYPE type, std::vector<std::shared_ptr<SceneObject>> targets)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->isMultiObject = true;
    cmd->objType = 0;
    int size = targets.size();
    for (int i = 0; i < size; i++) {
        cmd->uuids.push_back(targets[i]->getUUID());
        auto uuid_parent = targets[i]->getParent() != nullptr ? targets[i]->getParent()->getUUID() : "";
        cmd->uuid_parents.push_back(uuid_parent);
        json jObj;
        targets[i]->to_json(jObj);
        cmd->jObjs.push_back(jObj);
    }
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}

void CommandManager::PushCommand(COMMAND_TYPE type, std::vector<std::shared_ptr<SceneObject>> targets, std::vector<json> jObjs)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->isMultiObject = true;
    cmd->objType = 0;
    int size = targets.size();
    for (int i = 0; i < size; i++) {
        cmd->uuids.push_back(targets[i]->getUUID());
        auto pa_id = targets[i]->getParent() != nullptr ? targets[i]->getParent()->getUUID() : "";
        cmd->uuid_parents.push_back(pa_id);
        cmd->jObjs.push_back(jObjs[i]);
    }
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}

//AnimatorController
void CommandManager::PushCommand(COMMAND_TYPE type, std::shared_ptr<AnimatorController> target)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->objType = 1;
    cmd->uuid = target->getPath();
    json jObj;
    target->to_json(jObj);
    cmd->jObj = jObj;
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}

void CommandManager::PushCommand(COMMAND_TYPE type, std::shared_ptr<AnimatorController> target, json& jObj)
{
    int currentSize = m_undo.size();
    if (currentSize == m_maxSize)
    {
        //Remove the oldest 
        m_undo.erase(m_undo.begin());
    }
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    cmd->objType = 1;
    cmd->uuid = target->getPath();
    cmd->jObj = jObj;
    cmd->type = type;
    m_undo.push_back(cmd);
    m_redo.clear();
}


void CommandManager::Undo()
{
    if (m_undo.size() == 0) return;
    auto item = m_undo.back();
    if (item->objType == 1) {
        auto newItem = generateRevertAnimation(item);
        if(newItem != nullptr)
            m_redo.push_back(newItem);
    }
    else {
        if (item->type == COMMAND_TYPE::EDIT_COMPONENT) {
            auto newItem = generateComponent(item);
            m_redo.push_back(newItem);
        }
        else {
            m_redo.push_back(item);
        }
    }
    m_undo.pop_back();
    activeUndoCommand(item);
}

void CommandManager::Redo()
{
    if (m_redo.size() == 0) return;
    auto item = m_redo.back();
    if (item->objType == 1) {
        auto newItem = generateRevertAnimation(item);
        if (newItem != nullptr)
            m_undo.push_back(newItem);
    }
    else {
        if (item->type == COMMAND_TYPE::EDIT_COMPONENT) {
            auto newItem = generateComponent(item);
            m_undo.push_back(newItem);
        }
        else {
            m_undo.push_back(item);
        }
    }
    m_redo.pop_back();
    activeRedoCommand(item);
}

std::shared_ptr<Command> CommandManager::generateComponent(std::shared_ptr<Command> command)
{
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    auto scene = SceneManager::getInstance()->getCurrentScene();
    if (!command->isMultiObject) {
        auto target = scene->findObjectByUUID(command->uuid);
        json jObj;
        target->to_json(jObj);
        cmd->objType = 0;
        cmd->uuid = command->uuid;
        cmd->uuid_parent = command->uuid_parent;
        cmd->jObj = jObj;
    }
    else {
        int size = cmd->uuids.size();
        for (int i = 0; i < size; i++)
        {
            auto uuid = command->uuids[i];
            auto target = scene->findObjectByUUID(uuid);
            json jObj;
            target->to_json(jObj);
            cmd->jObjs.push_back(jObj);
            cmd->uuids.push_back(uuid);
            cmd->uuid_parents.push_back(command->uuid_parents[i]);
        }
        cmd->objType = 0;
        cmd->isMultiObject = command->isMultiObject;
    }
    cmd->type = command->type;  
    return cmd;
}

std::shared_ptr<Command> CommandManager::generateRevertAnimation(std::shared_ptr<Command> command)
{
    std::shared_ptr<Command> cmd = std::make_shared<Command>();
    auto controller = Editor::getInstance()->getCurrentAnimator();
    if (controller == nullptr) return nullptr;
    if (controller->getPath() != command->uuid) return nullptr;
    json jObj;
    controller->to_json(jObj);
    cmd->uuid = controller->getPath();
    cmd->jObj = jObj;
    cmd->type = command->type;
    cmd->objType = 1;
    return cmd;
}

void CommandManager::activeUndoCommand(std::shared_ptr<Command> command)
{
    if (command->objType == 0)
        activeUndoCommandSceneObject(command);
    else if (command->objType == 1)
        activeUndoCommandAnimationObject(command);
}

void CommandManager::activeRedoCommand(std::shared_ptr<Command> command)
{
    if (command->objType == 0)
        activeRedoCommandSceneObject(command);
    else if (command->objType == 1)
        activeRedoCommandAnimationObject(command);
}

void CommandManager::activeUndoCommandSceneObject(std::shared_ptr<Command> command)
{
    auto scene = SceneManager::getInstance()->getCurrentScene();
    auto type = command->type;
    switch (type) {
    case COMMAND_TYPE::ADD_OBJECT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr)
                {
                    Editor::getInstance()->removeTarget(target);
                    Editor::getCurrentScene()->removeObject(target);
                }
            }
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
            if (target != nullptr)
            {
                Editor::getInstance()->removeTarget(target);
                Editor::getCurrentScene()->removeObject(target);
            }
        }
    }
    break;
    case COMMAND_TYPE::DELETE_OBJECT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto newObject = scene->createObject(command->jObjs[i].value("name", std::string()), nullptr, command->jObjs[i].value("gui", false));
                newObject->from_json(command->jObjs[i]);
                auto parentx = scene->findObjectByUUID(command->uuid_parents[i]);
                newObject->setParent(parentx);
            }
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
            auto parent = scene->findObjectByUUID(command->uuid_parent);

            auto newObject = scene->createObject(command->jObj.value("name", std::string()), nullptr, command->jObj.value("gui", false));
            newObject->from_json(command->jObj);
            newObject->setParent(parent);
        }
    }
    break;
    case COMMAND_TYPE::SELECT_OBJECT:
        break;
    case COMMAND_TYPE::ADD_COMPONENT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr) {
                    auto j = command->jObjs[i];
                    auto jComps = j.at("comps");
                    for (auto it : jComps)
                    {
                        std::string key = it.at(0);
                        target->removeComponent(key);
                    }
                }
            }
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
            auto parent = scene->findObjectByUUID(command->uuid_parent);
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
    }
    break;
    case COMMAND_TYPE::DELETE_COMPONENT:
    {
        if (command->isMultiObject) {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr)
                {
                    auto j = command->jObjs[i];
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
        }
        else {
            auto target = scene->findObjectByUUID(command->uuid);
            auto parent = scene->findObjectByUUID(command->uuid_parent);
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
    }
    break;
    case COMMAND_TYPE::EDIT_COMPONENT:
    {
        if (command->isMultiObject) {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr)
                {
                    auto j = command->jObjs[i];
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
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
            auto parent = scene->findObjectByUUID(command->uuid_parent);
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
    }
    break;
    case COMMAND_TYPE::NONE:
    default:
        break;
    }

}

void CommandManager::activeRedoCommandSceneObject(std::shared_ptr<Command> command)
{
    auto scene = SceneManager::getInstance()->getCurrentScene();

    auto type = command->type;
    switch (type) {
    case COMMAND_TYPE::ADD_OBJECT:
    {
        if (command->isMultiObject) {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                auto parent = scene->findObjectByUUID(command->uuid_parents[i]);

                auto newObject = scene->createObject(command->jObjs[i].value("name", std::string()), nullptr, command->jObjs[i].value("gui", false));
                newObject->from_json(command->jObj);
                newObject->setParent(parent);
            }
        }
        else {
            auto target = scene->findObjectByUUID(command->uuid);
            auto parent = scene->findObjectByUUID(command->uuid_parent);

            auto newObject = scene->createObject(command->jObj.value("name", std::string()), nullptr, command->jObj.value("gui", false));
            newObject->from_json(command->jObj);
            newObject->setParent(parent);
        }
    }
    break;
    case COMMAND_TYPE::DELETE_OBJECT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr)
                {
                    Editor::getInstance()->removeTarget(target);
                    Editor::getCurrentScene()->removeObject(target);
                }
            }
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
            if (target != nullptr)
            {
                Editor::getInstance()->removeTarget(target);
                Editor::getCurrentScene()->removeObject(target);
            }
        }
    }
    break;
    case COMMAND_TYPE::SELECT_OBJECT:
        break;
    case COMMAND_TYPE::ADD_COMPONENT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr)
                {
                    auto j = command->jObjs[i];
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
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
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
    }
    break;
    case COMMAND_TYPE::DELETE_COMPONENT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr)
                {
                    auto j = command->jObjs[i];
                    auto jComps = j.at("comps");
                    for (auto it : jComps)
                    {
                        std::string key = it.at(0);
                        target->removeComponent(key);
                    }
                }
            }
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
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
    }
    break;
    case COMMAND_TYPE::EDIT_COMPONENT:
    {
        if (command->isMultiObject)
        {
            int size = command->uuids.size();
            for (int i = 0; i < size; i++) {
                auto target = scene->findObjectByUUID(command->uuids[i]);
                if (target != nullptr) {
                    auto j = command->jObjs[i];
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
        }
        else
        {
            auto target = scene->findObjectByUUID(command->uuid);
            if (target != nullptr) {
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
    }
    break;
    case COMMAND_TYPE::NONE:
    default:
        break;
    }

}

void CommandManager::activeUndoCommandAnimationObject(std::shared_ptr<Command> command)
{
    auto controller = Editor::getInstance()->getCurrentAnimator();
    if (controller == nullptr) return;
    if (controller->getPath() != command->uuid) return;
    controller->restore_from_json(command->jObj);

    /*auto type = command->type;
    switch (type)
    {
    case ige::creator::COMMAND_TYPE::ANIMATOR_ADD:
        break;
    case ige::creator::COMMAND_TYPE::ANIMATOR_REMOVE:
        break;
    case ige::creator::COMMAND_TYPE::ANIMATOR_EDIT:
        break;
    default:
        break;
    }*/

}

void CommandManager::activeRedoCommandAnimationObject(std::shared_ptr<Command> command)
{
    auto controller = Editor::getInstance()->getCurrentAnimator();
    if (controller == nullptr) return;
    if (controller->getPath() != command->uuid) return;
    controller->restore_from_json(command->jObj);
}


NS_IGE_END