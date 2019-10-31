#pragma once

#include "../base/common.h"
#include "../base/basedef.h"

class Lex
{
public:
	Lex()
		: line_(0)
		, cur_char_(0)
		, is_found_token_(false)
	{

	}
	bool OpenFile(const char* file_name);
	void CloseFile();
	Token NextToken();
private:
	void NextChar();
	void GenToken(TokenType token_type);
	void Error(std::string msg);
	bool ParseString();
	bool ParseNumber();
	int CheckReserved();
public:
	FileReader reader_;
	std::string buff_;
	int line_;
	char cur_char_;
	bool is_found_token_;
	Token cur_token_;
};