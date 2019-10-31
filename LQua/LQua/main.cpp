#include <stdio.h>
#include <stdlib.h>
#include "test/test.h"
#include "base/state.h"
#include "base/const.h"

int main()
{
	InitConstants();

	// Test::TestArray();
	char a[3] = { 'g', 'l', '\0' };
	printf("str len %d\n", ::strlen(a));

	LQState lqstate;
	lqstate.ExecuteFile("test.lua");

#ifdef WIN32
	getchar();
#endif

	return 0;
}