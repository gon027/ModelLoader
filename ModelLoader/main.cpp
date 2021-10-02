#include <iostream>
using namespace std;

#include "ModelLoader/PMD/PMDLoader.hpp"
#include "ModelLoader/PMX/PMXLoader.hpp"

int main()
{
	using namespace model;

	/*
	IModelLoader* pmdLoader = new pmd::PMDLoader{};
	if (!pmdLoader->load("TestModel/Shachiku/", "�В{����� Ver3.1.pmd", "tex")) {
		std::cout << "�_���[" << std::endl;
		return -1;
	}
	delete pmdLoader;
	*/

	IModelLoader* pmxLoader = new pmx::PMXLoader{};
	if (!pmxLoader->load("", "TestModel/SiroDanceCostume_v1.0/siro_dance_costume_v1.0.pmx", "tex")) {
		std::cout << "�_���[" << std::endl;
		return -1;
	}

	delete pmxLoader;
}

