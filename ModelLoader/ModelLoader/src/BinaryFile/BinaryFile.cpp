#include "../../include/BinaryFile/BinaryFile.hpp"
#include <iostream>
#include <Windows.h>

BinaryFile::BinaryFile()
	: ifs{}
	, position{ 0 }
	, size{ 0 }
{
}

BinaryFile::BinaryFile(const std::string _filePath)
	: ifs{ _filePath, std::ios::in | std::ios::binary }
	, position{ 0 }
	, size{ 0 }
{
	// バイト数の計算
	ifs.seekg(0, std::ios_base::end);
	{
		size = ifs.tellg();
	}
	ifs.seekg(0, std::ios_base::beg);
}

bool BinaryFile::open(const std::string& _filePath)
{
	ifs.open(_filePath, std::ios::in | std::ios::binary);

	// バイト数の計算
	ifs.seekg(0, std::ios_base::end);
	{
		size = ifs.tellg();
	}
	ifs.seekg(0, std::ios_base::beg);

	return isOpen();
}

void BinaryFile::read(char* _src, std::streamsize _count)
{
	ifs.read(_src, _count);
	position += _count;
}

char BinaryFile::read8()
{
	char value{ 0 };
	ifs.read((char*)&value, sizeof(char));
	position += sizeof(char);
	return value;
}

int BinaryFile::read16()
{
	int value{ 0 };
	ifs.read((char*)&value, sizeof(char) * 2);
	position += sizeof(char) * 2;
	return value;
}

int BinaryFile::read32()
{
	int value{ 0 };
	ifs.read((char*)&value, sizeof(char) * 4);
	position += sizeof(char) * 4;
	return value;
}

long long BinaryFile::read64()
{
	long long value{ 0 };
	ifs.read((char*)&value, sizeof(char) * 8);
	position += sizeof(char) * 8;
	return value;
}

float BinaryFile::readFloat() 
{
	float value{ 0 };
	ifs.read((char*)&value, sizeof(char) * 4);
	position += sizeof(char) * 4;
	return value;
}

double BinaryFile::readDouble()
{
	double value{ 0 };
	ifs.read((char*)&value, sizeof(char) * 8);
	position += sizeof(char) * 8;
	return value;
}

void BinaryFile::seek(std::streamsize _pos)
{
	ifs.seekg(_pos);
	position = _pos;
}

bool BinaryFile::isOpen() const
{
	return !isFail();
}

bool BinaryFile::isFail() const
{
	return ifs.fail();
}

bool BinaryFile::isEof() const
{
	return ifs.eof();
}
