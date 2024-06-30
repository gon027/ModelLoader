#include "../../include/BinaryFile/BinaryFile.hpp"
#include <Windows.h>

BinaryFile::BinaryFile()
	: ifs{}
	, position()
{
}

BinaryFile::BinaryFile(const std::string _filePath)
	: ifs{ _filePath, std::ios::in | std::ios::binary }
	, position()
{
}

bool BinaryFile::open(const std::string& _filePath)
{
	ifs.open(_filePath, std::ios::in | std::ios::binary);
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
