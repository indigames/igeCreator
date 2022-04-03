#pragma once
#include <utils/PyxieHeaders.h>
#include <utils/Singleton.h>
#include "components/Component.h"
#include "core/Macros.h"
#include <vector>
#include "core/Command.h"

#include "components/animation/AnimatorController.h"


using namespace ige::scene;
namespace ige::creator
{
	class CommandManager : public Singleton<CommandManager>
	{
	public:
		CommandManager();
		virtual ~CommandManager();
		
		// SceneObject
		void PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target);
		void PushCommand(COMMAND_TYPE type, std::shared_ptr<SceneObject> target, json& jObj);
		
		void PushCommand(COMMAND_TYPE type, std::vector<std::shared_ptr<SceneObject>> targets);
		void PushCommand(COMMAND_TYPE type, std::vector<std::shared_ptr<SceneObject>> targets, std::vector<json> jObjs);

		void PushCommand(COMMAND_TYPE type, std::shared_ptr<AnimatorController> target);
		void PushCommand(COMMAND_TYPE type, std::shared_ptr<AnimatorController> target, json& jObj);


		void Undo();
		void Redo();
	protected:
		std::shared_ptr<Command> generateComponent(std::shared_ptr<Command> command);

		void activeUndoCommand(std::shared_ptr<Command> command);
		void activeRedoCommand(std::shared_ptr<Command> command);

		void activeUndoCommandSceneObject(std::shared_ptr<Command> command);
		void activeRedoCommandSceneObject(std::shared_ptr<Command> command);

		void activeUndoCommandAnimationObject(std::shared_ptr<Command> command);
		void activeRedoCommandAnimationObject(std::shared_ptr<Command> command);

		std::shared_ptr<Command> generateRevertAnimation(std::shared_ptr<Command> command);

	protected:
		std::vector<std::shared_ptr<Command>> m_undo;
		std::vector<std::shared_ptr<Command>> m_redo;

		int m_maxSize = 30;
	};
}