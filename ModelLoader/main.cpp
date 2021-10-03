#include <iostream>
#include <cstring>
#include <algorithm>
#include <memory>
using namespace std;

#include "ModelLoader/ModelImporter.hpp"

int main()
{
	using namespace model;

	ModelImporter::getIns()->loadModel("Miku", "TestModel/Miku/", "LatŽ®ƒ~ƒNVer2.31_Normal.pmd");
	ModelImporter::getIns()->loadModel("Siro", "TestModel/SiroDanceCostume_v1.0/", "siro_dance_costume_v1.0.pmx");
	ModelImporter::getIns()->loadModel("Shachiku", "TestModel/Shachiku/", "ŽÐ’{‚¿‚á‚ñ Ver3.1.pmd");
		
	auto modelData1 = ModelImporter::getIns()->getModelData("Siro");
	auto modelData2 = ModelImporter::getIns()->getModelData("Miku");
	auto modelData3 = ModelImporter::getIns()->getModelData("Shachiku");

	cout << 1 << endl;

}

