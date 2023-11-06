#pragma once

#include <memory>

namespace model::fbx {

	enum class FBXPropertyType : char {
		String = 'S',
		Int16 = 'Y',
		Int32 = 'I',
		Int64 = 'L',
		Float = 'F',
		Double = 'D',
		Bool = 'C',
		VecUInt8 = 'R',
		VecInt32 = 'i',
		VecInt64 = 'l',
		VecFloat = 'f',
		VecDouble = 'd',
		VecBool = 'b',
	};

	class IFBXProperty {
	public:
		virtual FBXPropertyType getType() = 0;
	};

	class FBXStringProperty : public IFBXProperty {
	public:
		FBXStringProperty(const std::string& _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::String;
		}

		std::string getValue() const {
			return value;
		}

	private:
		std::string value;
	};

	class FBXInt16Property : public IFBXProperty {
	public:
		FBXInt16Property(int _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::Int16;
		}

		short getValue() const {
			return value;
		}

	private:
		int value;
	};

	class FBXInt32Property : public IFBXProperty {
	public:
		FBXInt32Property(int _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::Int32;
		}

		int getValue() const {
			return value;
		}

	private:
		int value;
	};

	class FBXInt64Property : public IFBXProperty {
	public:
		FBXInt64Property(long long _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::Int64;
		}

		long long getValue() const {
			return value;
		}

	private:
		long long value;
	};

	class FBXFloatProperty : public IFBXProperty {
	public:
		FBXFloatProperty(float _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::Float;
		}

		float getValue() const {
			return value;
		}

	private:
		float value;
	};

	class FBXDoubleProperty : public IFBXProperty {
	public:
		FBXDoubleProperty(double _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::Double;
		}

		double getValue() const {
			return value;
		}

	private:
		double value;
	};

	class FBXBoolProperty : public IFBXProperty {
	public:
		FBXBoolProperty(bool _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::Bool;
		}

		bool getValue() const {
			return value;
		}

	private:
		bool value;
	};

	class FBXInt32ArrayProperty : public IFBXProperty {
	public:
		FBXInt32ArrayProperty(const std::vector<int>& _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::VecInt32;
		}

		std::vector<int>& getValue() {
			return value;
		}

	private:
		std::vector<int> value;
	};

	class FBXInt64ArrayProperty : public IFBXProperty {
	public:
		FBXInt64ArrayProperty(const std::vector<long long>& _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::VecInt64;
		}

		std::vector<long long> getValue() {
			return value;
		}

	private:
		std::vector<long long> value;
	};

	class FBXFloatArrayProperty : public IFBXProperty {
	public:
		FBXFloatArrayProperty(const std::vector<float>& _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::VecFloat;
		}

		std::vector<float> getValue() {
			return value;
		}

	private:
		std::vector<float> value;
	};

	class FBXDoubleArrayProperty : public IFBXProperty {
	public:
		FBXDoubleArrayProperty(const std::vector<double>& _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::VecDouble;
		}

		std::vector<double> getValue() {
			return value;
		}

	private:
		std::vector<double> value;
	};

	class FBXBoolArrayProperty : public IFBXProperty {
	public:
		FBXBoolArrayProperty(const std::vector<char>& _value)
			: value{ _value }
		{}

		FBXPropertyType getType() override {
			return FBXPropertyType::VecBool;
		}

		std::vector<char> getValue() {
			return value;
		}

	private:
		std::vector<char> value;
	};

	using FBXPropertyPtr = std::shared_ptr<IFBXProperty>;
	using WeakFBXPropertyPtr = std::weak_ptr<IFBXProperty>;


	template <class Value>
	inline Value getPropertyValue(FBXPropertyPtr _ptr) {};

	template <> inline std::string getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXStringProperty>(_ptr)->getValue();
	}
	
	template <> inline short getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXInt16Property>(_ptr)->getValue();
	}

	template <> inline int getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXInt32Property>(_ptr)->getValue();
	}

	template <> inline long long getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXInt64Property>(_ptr)->getValue();
	}

	template <> inline float getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXFloatProperty>(_ptr)->getValue();
	}

	template <> inline double getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXDoubleProperty>(_ptr)->getValue();
	}

	template <> inline bool getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXBoolProperty>(_ptr)->getValue();
	}

	template <> inline std::vector<int>  getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXInt32ArrayProperty>(_ptr)->getValue();
	}

	template <> inline std::vector<long long>  getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXInt64ArrayProperty>(_ptr)->getValue();
	}

	template <> inline std::vector<float>  getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXFloatArrayProperty>(_ptr)->getValue();
	}

	template <> inline std::vector<double>  getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXDoubleArrayProperty>(_ptr)->getValue();
	}

	template <> inline std::vector<char>  getPropertyValue(FBXPropertyPtr _ptr) {
		return std::dynamic_pointer_cast<FBXBoolArrayProperty>(_ptr)->getValue();
	}
}