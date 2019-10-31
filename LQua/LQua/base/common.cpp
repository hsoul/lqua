#include "common.h"

bool FileReader::Open(const char* file_name)
{
	file_ = ::fopen(file_name, "rb");
	return (nullptr != file_);
}

void FileReader::Close()
{
	if (file_)
		::fclose(file_);
	file_ = nullptr;
}

char FileReader::NextChar()
{
	if (nullptr == file_)
		return LEX_EOZ;
	if (cur_ >= length_)
	{
		cur_ = 0;
		length_ = ::fread(content_, 1, sizeof(content_), file_);
		if (length_ <= 0)
			return LEX_EOZ;
	}
	return content_[cur_++];
}

bool FileWriter::Open(const char* file_name)
{
	file_ = ::fopen(file_name, "W+");
	return (nullptr != file_);
}

void FileWriter::Close()
{
	if (file_)
		::fclose(file_);
	file_ = nullptr;
}

void FileWriter::Write(const char* str, int length)
{
	if (file_)
	{
		::fwrite(str, 1, length, file_);
		::fflush(file_);
	}
}

static const unsigned char g_log2[256] = {
	0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

int BaseFunc::CeilLog2(unsigned int x)
{
	x -= 1;

	int n = 0;
	while (x >= 256) {
		n += 8;
		x >>= 8;
	}
	return n + g_log2[x];
}

int BaseFunc::SquareSize(int size, int base)
{
	while (base < size)
	{
		base *= 2;
	}
	return base;
}

HashNum BaseFunc::GenNumHash(Number n)
{
	return (HashNum)((n - (int)n) * 1000000 + n);
}

HashNum BaseFunc::GenStringHash(const char* s, int length)
{
	HashNum h = length;
	int step = (length >> 5) + 1;
	for (int i = 0; i < length; i += step) {
		h = h ^ ((h << 5) + (h >> 2) + (unsigned char)s[i]);
	}
	return h;
}

