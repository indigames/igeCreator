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
	};

	class Command
	{
	public:
		Command() {}
		virtual ~Command() {}
	public:
		std::string uuid;
		std::string uuid_parent;
		json jObj;
		COMMAND_TYPE type;
	};
}