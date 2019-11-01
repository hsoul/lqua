#pragma once

#include "../base/ldata.h"
#include "../base/lobject.h"
#include "../base/basedef.h"

class LQState;

class VM
{
public:
	static int Run(LQState* ls, ObjectProto* proto);
private:
	static int Execute(LQState* ls, ObjectProto* proto, int base);
	static int CallCfunc(LQState* ls, CFunction func, int base);
	static Number Caculate(Number n1, Number n2, OpCode op_code);
	static bool Compare(Number n1, Number n2, OpCode op_code);
	static void Concat(LQState* ls, const TObject& o1, const TObject& o2, TObject& result);
	static void Call(LQState* ls, int param_nums, int result_nums);
	static void AdjustTop(LQState* ls, int new_top);
	static int GetLine(ObjectProto* proto, int pc);
};