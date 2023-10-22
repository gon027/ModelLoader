#include "FBXLoader.hpp"

#include <iostream>

namespace model {

    fbx::FBXLoader::FBXLoader()
        : bf{}
        , version{}
    {
    }

    fbx::FBXLoader::~FBXLoader()
    {
    }

    bool fbx::FBXLoader::load(const std::string& _modelDir)
    {
        bf.open(_modelDir);
        if (bf.isFail()) {
            return false;
        }

        char magicNumber[32];
        bf.read(magicNumber, sizeof(char) * 23);
        std::cout << "�}�W�b�N�i���o�[ : " << magicNumber << std::endl;

        version = bf.read32();
        std::cout << "�o�[�W���� : " << version << std::endl;

        return false;
    }

}