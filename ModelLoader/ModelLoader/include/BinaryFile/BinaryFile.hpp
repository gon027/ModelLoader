#pragma once

#include <string>
#include <fstream>

class BinaryFile {
public:
	BinaryFile();
	BinaryFile(const std::string _filePath);
	~BinaryFile() = default;

	bool open(const std::string& _filePath);

	void read(char* _src, std::streamsize _count);

	char read8();

	int read16();

	int read32();

	long long read64();

	float readFloat();

	double readDouble();

	void seek(std::streamsize _pos);

	bool isOpen() const;

	bool isFail() const;

	bool isEof() const;

	inline long long getPosition() const {
		return position;
	}

	inline long long getSize() const {
		return size;
	}

	inline long long getTellg() {
		return ifs.tellg();
	}

private:
	BinaryFile(const BinaryFile&);
	BinaryFile& operator=(const BinaryFile&);

private:
	std::ifstream ifs;
	long long position;
	long long size;
};