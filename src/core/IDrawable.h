#pragma once

namespace ige::creator
{
    //! Interface IDrawable
	class IDrawable
	{
	public:
		virtual void draw() = 0;
	};
}