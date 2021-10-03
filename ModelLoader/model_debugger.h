#pragma once

#include <iostream>
using namespace std;


#include "ModelLoader/PMD/PMDLoader.hpp"
#include "ModelLoader/PMX/PMXLoader.hpp"

#include "ModelLoader/PMD/PMDFile.hpp"
#include "ModelLoader/PMX/PMXFile.hpp"

inline void pmxDebug(model::pmx::PMXLoader* _pmx) {
	auto& file = _pmx->getFile();

	for (int i = 0; i < 5; ++i) {
		auto v = file.vertexes.data();
		std::cout << v[i].position[0]  << std::endl;
	}

}

inline void pmdDebug(model::pmd::PMDLoader* _pmd) {
	auto& file = _pmd->getFile();

	std::cout << file.header.modelName << std::endl;
	std::cout << file.header.version << std::endl;
	std::cout << file.header.comment << std::endl;
	std::cout << file.header.name << std::endl;

}