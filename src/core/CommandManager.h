#pragma once
#include <utils/PyxieHeaders.h>
#include <utils/Singleton.h>
#include "components/Component.h"
#include "core/Macros.h"
#include <vector>
#include "core/Command.h"

using namespace ige::scene;
namespace ige::creator
{
	class CommandManager : public Singleton<CommandManager>
	{
	public:
		CommandManager();
		virtual ~CommandManager();
		void PushCommand(std::shared_ptr<SceneObject> target, COMMAND_TYPE type = COMMAND_TYPE::NONE);
		void Undo();
		void Redo();
	protected:
		
		void activeCommand(std::shared_ptr<Command> command);
	protected:
		std::vector<std::shared_ptr<Command>> m_undo;
		std::vector<std::shared_ptr<Command>> m_redo;

		int m_maxSize = 30;
	};
}