#pragma once

#include "../base/ldata.h"

class LQState;

class VM
{
public:
	static int Run(LQState* ls, ObjectProto* proto);
private:
};