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

void CommandManager::PushCommand(std::shared_ptr<SceneObject> target, COMMAND_TYPE type)
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
    pyxie_printf("PUSH COMMAND %d \n", ((int)m_undo.size()));
}

void CommandManager::Undo()
{
    pyxie_printf("Undo %d\n", ((int)m_undo.size()));
    if (m_undo.size() == 0) return;
    auto item = m_undo.back();
    m_undo.pop_back();
    m_redo.push_back(item);
    activeCommand(item);
}

void CommandManager::Redo()
{
    if (m_redo.size() == 0) return;
    auto item = m_redo.back();
    m_redo.pop_back();
    m_undo.push_back(item);
    activeCommand(item);
}

void CommandManager::activeCommand(std::shared_ptr<Command> command)
{
    auto scene = SceneManager::getInstance()->getCurrentScene();
    auto target = scene->findObjectByUUID(command->uuid);
    auto parent = scene->findObjectByUUID(command->uuid_parent);
    
    auto type = command->type;
    pyxie_printf("ACTIVE COMMAND %d \n", type);
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
        break;
    case COMMAND_TYPE::DELETE_COMPONENT:
        break;
    case COMMAND_TYPE::EDIT_COMPONENT:
        {
            if (parent != nullptr) {
                if (target != nullptr)
                {
                    target->restore_json(command->jObj);
                    pyxie_printf("Restrore %s \n", command->jObj.value("name", std::string()));
                }
                else
                {
                    auto newObject = scene->createObject(command->jObj.value("name", std::string()), nullptr, command->jObj.value("gui", false));
                    newObject->from_json(command->jObj);
                    newObject->setParent(parent);
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