#pragma once

#include "basedef.h"

class FileReader
{
public:
	FileReader()
		: file_(nullptr)
		, length_(0)
		, cur_(0)
	{
		memset(content_, 0, sizeof(content_));
	}
	bool Open(const char* file_name);
	void Close();
	char NextChar();
public:
	FILE* file_;
	char content_[STD_BUFFER_LEN];
	int length_;
	int cur_;
};

class FileWriter
{
public:
	FileWriter()
		: file_(nullptr)
	{

	}
	bool Open(const char* file_name);
	void Close();
	void Write(const char* str, int length);
public:
	FILE* file_;
};

class BaseFunc
{
public:
	static int CeilLog2(unsigned int x); // 指数向上取整
	static int SquareSize(int size, int base); // first value < 2(n) 次方
	static HashNum GenNumHash(Number n);
	static HashNum GenStringHash(const char* s, int length);
};

class RuntimeError
{
public:
	RuntimeError(const std::string& message, int line_num)
		: msg_(message)
		, line_(line_num)
	{

	}
public:
	std::string msg_;
	int line_;
};