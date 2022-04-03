#pragma once
#include <utils/PyxieHeaders.h>
using namespace pyxie;

#include "scene/Scene.h"
using namespace ige::scene;

namespace ige::creator
{
	enum class COMMAND_TYPE
	{
		NONE = 0,
		SELECT_OBJECT = 1,
		ADD_OBJECT,
		DELETE_OBJECT,
		ADD_COMPONENT,
		DELETE_COMPONENT,
		EDIT_COMPONENT,

		//ANIMATOR EDITOR
		ANIMATOR,
		ANIMATOR_ADD,
		ANIMATOR_REMOVE,
		ANIMATOR_EDIT,

	};

	class Command
	{
	public:
		Command() {}
		virtual ~Command() {}
	public:
		int objType = 0;
		std::string uuid;
		std::string uuid_parent;
		
		std::vector<std::string> uuids;
		std::vector<std::string> uuid_parents;
		
		json jObj;
		std::vector<json> jObjs;

		bool isMultiObject = false;

		COMMAND_TYPE type;

	};
}