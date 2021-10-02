#include <iostream>
using namespace std;

#include "ModelLoader/PMD/PMDLoader.hpp"
#include "ModelLoader/PMX/PMXLoader.hpp"

int main()
{
	using namespace model;

	IModelLoader* pmdLoader = new pmx::PMXLoader{};
	if (!pmdLoader->load("TestModel/Shachiku/ŽÐ’{‚¿‚á‚ñ Ver3.1.pmd", "tex")) {
		std::cout << "ƒ_ƒ[" << std::endl;
		return -1;
	}

	delete pmdLoader;
}

