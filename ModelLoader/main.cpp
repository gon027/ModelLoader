#include <iostream>
#include <cstring>
#include <algorithm>
#include <memory>
using namespace std;

#include "ModelLoader/PMD/PMDLoader.hpp"
#include "ModelLoader/PMX/PMXLoader.hpp"

#include "model_debugger.h"
#include "ModelLoader/ModelData.hpp"

#include "ModelLoader/ModelImporter.hpp"

int main()
{
	using namespace model;
	std::vector<ModelData> ms;

	ModelImporter::getIns()->loadModel("Miku", "TestModel/Miku/", "LatŽ®ƒ~ƒNVer2.31_Normal.pmd", "tex");

	//auto& m = ModelImporter::getIns()->getModelData("Miku");
		
	{
		ModelData& modelData = ModelImporter::getIns()->getModelData("Miku");;
		ms.emplace_back(modelData);
		
		{
			cout << std::get<0>(modelData.modelName) << endl;

			for (size_t i{ 0 }; i < 10; ++i) {
				std::cout << "x = " << modelData.vertexes[i].position[0] << std::endl;
				std::cout << "y = " << modelData.vertexes[i].position[1] << std::endl;
				std::cout << "z = " << modelData.vertexes[i].position[2] << std::endl;
			}

			for (int i = 0; i < 10; ++i) {
				cout << modelData.indexes[i] << endl;
			}

			for (size_t i{ 0 }; i < modelData.materials.size(); ++i) {
				cout << std::get<0>(modelData.materials[i].textureName) << endl;
				cout << modelData.materials[i].vertCount << endl;
			}
		}
	}
	

	//cout << std::get<0>(ms[0].modelName) << endl;
}

