#pragma once

#include <string>
#include <unordered_map>
#include <memory>
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

using ModelDataPtr = std::shared_ptr<model::ModelData>;
using ModelList = std::unordered_map<std::string, ModelDataPtr>;

class ModelImporter : public Singleton<ModelImporter> {
	friend Singleton<ModelImporter>;

public:
	void loadModel(const std::string& _name, const std::string& _modelDir, const std::string& _modelFile);

	ModelDataPtr getModelData(const std::string& _name);

	ModelDataPtr getModelData(const std::string& _name, const std::string& _modelDir, const std::string& _modelFile);

protected:
	ModelImporter();

private:
	ModelList modelList;
};