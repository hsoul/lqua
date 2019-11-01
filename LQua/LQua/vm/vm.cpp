#include "vm.h"

int VM::Run(LQState* ls, ObjectProto * proto)
{
	return 0;
}

int VM::Execute(LQState* ls, ObjectProto* proto, int base)
{

}

int VM::CallCfunc(LQState* ls, CFunction func, int base)
{

}

Number VM::Caculate(Number n1, Number n2, OpCode op_code)
{
	switch (op_code)
	{
	case OP_ADD:
		return n1 + n2;
	case OP_SUB:
		return n1 - n2;
	case OP_MUL:
		return n1 * n2;
	case OP_DIV:
		return n1 / n2;
	default:
		break;
	}
	return 0;
}

bool VM::Compare(Number n1, Number n2, OpCode op_code)
{

}

void VM::Concat(LQState* ls, const TObject& o1, const TObject& o2, TObject& result)
{

}

void VM::Call(LQState* ls, int param_nums, int result_nums)
{

}

void VM::AdjustTop(LQState* ls, int new_top)
{

}

int VM::GetLine(ObjectProto* proto, int pc)
{

}
