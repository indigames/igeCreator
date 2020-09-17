#pragma once

#include <FCollada.h>
#include <FCDocument/FCDSceneNode.h>
#include <FCDocument/FCDAnimated.h>
//#include <edge_stdint.h>
//#include <iostream>
//#include<sstream>
#include <string>
#include <vector>
//#include <map>
//#include <set>

namespace pyxie
{
	class FigureExportHelper {

		static FigureExportHelper instance;
		FigureExportHelper() {}

	public:
		static FigureExportHelper& Instance() { return instance; }

		void GetAnimateds(FCDSceneNode* pNode, FCDAnimatedList& animateds);
		void RenameSceneNode(std::vector<const FCDSceneNode*> joints, std::string fileName);

		std::string MakeLightName(std::string name, int type, int& no);
		uint32_t TriangulatePolygons(const int32_t *polygons, int32_t **outTriangles, const uint16_t* materials, uint16_t** outMaterials, const uint16_t* models, uint16_t** outModels);
	};
}