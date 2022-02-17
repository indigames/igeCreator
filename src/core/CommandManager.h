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
		void PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target);
		void PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target, json& jObj);
		void Undo();
		void Redo();
	protected:
		std::shared_ptr<Command> generateComponent(std::shared_ptr<Command> command);

		void activeUndoCommand(std::shared_ptr<Command> command);
		void activeRedoCommand(std::shared_ptr<Command> command);
	protected:
		std::vector<std::shared_ptr<Command>> m_undo;
		std::vector<std::shared_ptr<Command>> m_redo;

		int m_maxSize = 30;
	};
}