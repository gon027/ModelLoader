#pragma once

#include <string>
#include <unordered_map>
#include "ModelData.hpp"

template<class T>
class Singleton {
public:

	static T* getIns() {
		static T Instance;
		return &Instance;
	}

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

private:
	Singleton(const Singleton&);
	Singleton& operator= (const Singleton&);
	Singleton(Singleton&&);
	Singleton&& operator= (Singleton&&);
};

class ModelImporter : public Singleton<ModelImporter> {
	friend Singleton<ModelImporter>;

public:
	void loadModel(const std::string& _name, const std::string& _modelDir, const std::string& _modelFile, const std::string _texDir);

	model::ModelData& getModelData(const std::string& _name);

protected:
	ModelImporter();

private:
	std::unordered_map<std::string, model::ModelData> modelList;
};