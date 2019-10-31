#include "lex.h"
#include "../base/const.h"

bool Lex::OpenFile(const char* file_name)
{
	line_ = 1;
	bool result = reader_.Open(file_name);
	NextChar();
	return result;
}

void Lex::CloseFile()
{
	reader_.Close();
}

Token Lex::NextToken()
{
	is_found_token_ = false;
	buff_.clear();
	cur_token_.Reset();

	while (!is_found_token_)
	{
		switch (cur_char_)
		{
		case ' ': case '\t': case '\r':// 无效字符
		{
			NextChar();
			break;
		}

		case '\n':
		{
			++line_;
			NextChar();
			break;
		}

		case '+': case '*': case '/':
		case '(': case ')': case '[': case ']': case '{': case '}':
		case ',': case ';': // 单字符token
		{
			GenToken(g_char_tokens[cur_char_]);
			NextChar();
			break;
		}

		case '-':
		{
			NextChar();
			if ('-' == cur_char_) // 单行注释
			{
				do
				{
					NextChar();
				} while ('\n' != cur_char_ && LEX_EOZ != cur_char_);
			}
			else
				GenToken(TK_SUB_MINUS);
			break;
		}

		case '.': // 点号.，或字符串连接符..，或可变参数...
		{
			TokenType token_type = TK_POINT;
			NextChar();
			if ('.' == cur_char_)
			{
				NextChar();
				if ('.' == cur_char_)
				{
					token_type = TK_DOTS;
					NextChar();
				}
				else
					token_type = TK_CONCAT;
			}
			GenToken(token_type);
			break;
		}

		case '=': case '>': case '<': // 赋值=，或相等==，或大于>，或大于等于>=，或小于<，或小于等于<=
		{
			TokenType first_type = g_char_tokens[cur_char_];
			NextChar();
			if (cur_char_ == '=')
			{
				TokenType token_type = TK_EQ;
				if (TK_GT == first_type)
					token_type = TK_GE;
				else if (TK_LT == first_type)
					token_type = TK_LE;
				GenToken(token_type);
				NextChar();
			}
			else
				GenToken(first_type);
			break;
		}

		case '~':
		{
			NextChar();
			if ('=' != cur_char_)
			{
				GenToken(TK_NE);
				NextChar();
			}
			break;
		}

		case '"': case '\'': // 字符串， 双引号或单引号
		{
			ParseString();
			GenToken(TK_STRING);
			break;
		}

		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		{
			ParseNumber();
			GenToken(TK_NUMBER);
			break;
		}

		case LEX_EOZ: case NULL: // 文件结束符
		{
			GenToken(TK_EOF);
			break;
		}

		default:  // 标识符（下划线或字母开头）
		{
			if (cur_char_ == '_' || ::isalpha(cur_char_))
			{
				do
				{
					buff_.push_back(cur_char_);
					NextChar();
				} while (::isalnum(cur_char_) || cur_char_ == '_');
				buff_.push_back('\0');

				int r_index = CheckReserved();
				if (-1 == r_index)
					GenToken(TK_NAME);
				else
					GenToken(g_reserved[r_index].type_);
				break;
			}
			else
				Error("unknown character");
		}
		}
	}

	return cur_token_;
}

void Lex::NextChar()
{
	cur_char_ = reader_.NextChar();
}

void Lex::GenToken(TokenType token_type)
{
	cur_token_.token_type_ = token_type;
	cur_token_.token_str_ = buff_.c_str();
	cur_token_.line_ = line_;
	is_found_token_ = true;
}

void Lex::Error(std::string msg)
{
	throw RuntimeError(msg, line_);
}

bool Lex::ParseString()
{
	char delimiter = cur_char_;
	NextChar();
	while (cur_char_ != delimiter)
	{
		switch (cur_char_)
		{
		case LEX_EOZ: // 无效字符串
		case '\n':
		{
			Error("unfinished string");
			return false;
		}
		case '\\': // 转义字符
		{
			NextChar();
			switch (cur_char_)
			{
			case 'a':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case 'v':
			{
				buff_.push_back('\\'); // push \a \n \r
				buff_.push_back(cur_char_);
				NextChar();
				break;
			}
			default:
			{
				if (::isdigit(cur_char_)) // 十六进制数字
				{
					// \xxx
				}
				else
				{
					buff_.push_back(cur_char_); // \\ \' \"
					NextChar();
				}
				break;
			}
			}
		}
		default:
		{
			buff_.push_back(cur_char_); // noraml str : hello
			NextChar();
		}
		}
	}
	buff_.push_back('\0');
	NextChar();
	return true;
}

bool Lex::ParseNumber()
{
	do
	{
		buff_.push_back(cur_char_);
		NextChar();
	} while (::isdigit(cur_char_)); // 是否为数字， 整数部分

	if (cur_char_ = '.')
	{
		buff_.push_back(cur_char_);
		NextChar();
		while (::isdigit(cur_char_))
		{
			buff_.push_back(cur_char_);
			NextChar();
		}
	}
	buff_.push_back('\0');
	return true;
}

int Lex::CheckReserved()
{
	for (int i = 0; i < sizeof(g_reserved) / sizeof(ReservedToken); ++i)
	{
		if (nullptr == g_reserved[i].token_)
			break;
		if (0 == ::strcmp(g_reserved[i].token_, buff_.c_str()))
			return i;
	}
	return -1;
}
