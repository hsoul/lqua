#include <stdio.h>
#include <stdlib.h>
#include "test/test.h"
#include "base/state.h"
#include "base/const.h"

int main()
{
	InitConstants();

	// Test::TestArray();

	LQState lqstate;
	lqstate.ExecuteFile("test.lua");

#ifdef WIN32
	getchar();
#endif

	return 0;
}