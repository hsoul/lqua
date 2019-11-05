#pragma once

#include <string>
#include <sstream>

inline std::string ftoa(double f)
{
	std::ostringstream s;
	s << f;
	return s.str();
}

inline std::string itoa(int i)
{
	std::ostringstream s;
	s << i;
	return s.str();
}

inline std::string itoa(long long i)
{
	std::ostringstream s;
	s << i;
	return s.str();
}

template<int BUFF_SIZE>
struct StaticString
{
	StaticString()
	{
		length_ = 0;
		datas_[length_] = '\0';
	}
	StaticString(const char* str)
	{
		length_ = 0;
		datas_[length_] = '\0';
		SetStr(str);
	}
	void Clear() { length_ = 0; }
	void SetStr(const char* str)
	{
		if (str) 
		{
			length_ = strlen(str);
			if (length_ >= BUFF_SIZE)
			{
				length_ = BUFF_SIZE - 1;
			}
			strncpy(datas_, str, length_);
			datas_[length_] = '\0';
		}
	}
	void Append(char c) 
	{
		if (length_ < BUFF_SIZE) 
		{
			datas_[length_++] = c;
		}
	}
	void Format(const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		length_ = vsnprintf(datas_, BUFF_SIZE - 1, fmt, ap);
		va_end(ap);
		datas_[length_] = '\0';
	}
	bool operator == (const StaticString& other) const 
	{
		return length_ == other.length_ && strncmp(datas_, other.datas_, length_) == 0;
	}
	bool operator < (const StaticString& other) const 
	{
		if (length_ != other.length_)
		{
			return length_ < other.length_;
		}
		return (strncmp(datas_, other.datas_, length_) < 0 ? true : false);
	}
public:
	char datas_[BUFF_SIZE];
	int length_;
};

typedef StaticString<1024> ShortString;