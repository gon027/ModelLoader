#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "include/ModelData.hpp"

#ifdef DEF_SINGLETON

#include <singletoh.h>

#else

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

#endif // DEF_SINGLETON

struct ModelDesc {
	std::string modelDirectoy;
	std::string modelFileName;
};

class ModelImporter : public Singleton<ModelImporter> {
	friend Singleton<ModelImporter>;

public:
	void loadPMX(const std::string& _name, const ModelDesc& _modelDesc);

	void loadPMD(const std::string& _name, const ModelDesc& _modelDesc);

	void loadFBX(const std::string& _name, const ModelDesc& _modelDesc);

	ModelDataPtr getModelData(const std::string& _name);

protected:
	ModelImporter();

private:
	ModelList modelList;
};