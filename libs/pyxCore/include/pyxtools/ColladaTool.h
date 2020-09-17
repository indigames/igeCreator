#pragma once

#include	<FCollada.h>
#include	<FCDocument/FCDocument.h>
namespace pyxie
{
	class ColladaTool
	{
	public:
		ColladaTool(void);
		~ColladaTool(void);
		static void FreezeTransforms(FCDocument *document, bool freezeGeometory);
		static void SetPointLightScale(FCDocument *document);
	};
}