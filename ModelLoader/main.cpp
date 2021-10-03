#include <iostream>
#include <cstring>
#include <algorithm>
#include <memory>
using namespace std;

#include "ModelLoader/PMD/PMDLoader.hpp"
#include "ModelLoader/PMX/PMXLoader.hpp"

#include "model_debugger.h"
#include "ModelData.hpp"

int main()
{
	using namespace model;

	std::unique_ptr<pmd::PMDLoader> pmdLoader{ new pmd::PMDLoader{} };
	if (!pmdLoader->load("TestModel/Miku/", "Lat式ミクVer2.31_Normal.pmd", "tex")) {
		std::cout << "ダメー" << std::endl;
		return -1;
	}

	std::unique_ptr<pmx::PMXLoader> pmxLoader{ new pmx::PMXLoader{} };
	if (!pmxLoader->load("TestModel/SiroDanceCostume_v1.0/", "siro_dance_costume_v1.0.pmx", "tex")) {
		std::cout << "ダメー" << std::endl;
		return -1;
	}

	ModelData modelData = pmdLoader->getFile();
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
			cout << modelData.materials[i].vertCount   << endl;
		}
	}
}

