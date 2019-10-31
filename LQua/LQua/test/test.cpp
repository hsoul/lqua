#include "test.h"
#include "../base/memory.h"

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
}
