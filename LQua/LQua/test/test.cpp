#include "test.h"
#include "../base/memory.h"
#include <time.h>
#include "../base/dump.h"
#include <unordered_map>

UseTime::UseTime(const char* name)
{
	name_ = name;
	start_time_ = (int)::time(NULL);
}

UseTime::~UseTime()
{
	log("[UseTime] %s %ds\n", name_, (int)time(NULL) - start_time_);
}

void Test::TestArray()
{
	Array<char> arr;
	arr.Init(18, __FUNCTION__);
	arr.Add('h');
	arr.Add('e');
	arr.Add('l');
	arr.Add('l');
	arr.Add('o');
	arr.Add('w');
	arr.Add('\0');
	printf("%c\n", arr[5]);
	printf("%s\n", arr.datas_);
	arr.clear();
	std::unordered_map<int, std::string> mp;
	mp[1] = "sfsdf";
	mp[5] = 7;
}
