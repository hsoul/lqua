#include "vm.h"
#include <stdio.h>
#include "../base/ldata.h"
#include "../base/common.h"
#include "../base/state.h"

int VM::Run(LQState* ls, ObjectProto* proto)
{
	try
	{
		int base = 0;
		Execute(ls, proto, base);
	}
	catch (const RuntimeError& e)
	{
		printf("============= execute exception =============\n");
		printf("[FILE] %s:%d, [ERROR] %s\n", proto->file_name_.c_str(), e.line_, e.msg_.c_str());
	}
	return 0;
}

int VM::Execute(LQState* ls, ObjectProto* proto, int base)
{
	Instruction arg = 0;
	OpCode op_code = OP_NONE;
	bool is_stop = false;
	int first_result = base;
	int codes_size = proto->codes_.size();
	Array<Instruction>& codes = proto->codes_;

	try
	{
		for (int pc = VM_CODE_START; pc < codes_size && !is_stop; ++pc)
		{
			op_code = (OpCode)codes[pc];
			ls->cur_pc_ = pc;
			switch (op_code)
			{
			case OP_PUSH_CONSTANT:
			{
				arg = codes[++pc];
				stack_push_object(ls, proto->constants_[arg]);
				break;
			}
			case OP_ADD:
			{
				TObject& rval = stack_reverse(ls, -1);
				TObject& lval = stack_reverse(ls, -2);
				if (rval.object_type_ == ObjectType_Number && lval.object_type_ == ObjectType_Number)
				{
					Number result = Caculate(NUM_VAL(lval), NUM_VAL(rval), OpCode(op_code));
					stack_pop(ls); // 将元算数在栈上清除，并将计算结果压栈 
					NUM_VAL(stack_back(ls)) = result;
				}
				else
					ls->error("caculate operand isn't number");
				break;
			}
			case OP_SET_GLOBAL:
			{
				arg = codes[++pc];
				ls->globals_.Set(proto->constants_[arg], stack_back(ls));
				break;
			}
			case END_CODE:
			{
				int top = stack_top(ls);
				stack_pop_n(ls, top - base); // 弹出局部变量
				is_stop = true;
				printf("proto excute end\n");
				break;
			}
			case OP_CALL:
			{
				break;
			}
			default:
				break;
			}
		}
	}
	catch (RuntimeError& e)
	{
		e.line_ = GetLine(proto, ls->cur_pc_);
		printf("============= execute exception =============\n");
		printf("[FILE] %s:%d, [ERROR] %s\n", proto->file_name_.c_str(), e.line_, e.msg_.c_str());
	}

	return first_result;
}

int VM::CallCfunc(LQState* ls, CFunction func, int base)
{
	return 0;
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
	switch (op_code)
	{
	case OP_GT: return (n1 > n2 ? true : false);
	case OP_GE: return (n1 >= n2 ? true : false);
	case OP_LT: return (n1 < n2 ? true : false);
	case OP_LE: return (n1 <= n2 ? true : false);
	case OP_EQ: return (n1 == n2 ? true : false);
	case OP_NE: return (n1 != n2 ? true : false);
	default: break;
	}
	return false;
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
	int i = 0;
	int j = proto->pc_lines_.size() - 1;
	while (i < j)
	{
		int m = (i + j) / 2;
		LineInfo& line_info = proto->pc_lines_[m];
		if (pc == line_info.pc_)
			return line_info.line_;
		else if (pc < line_info.pc_)
			j = m - 1;
		else
			i = m + 1;
	}
	return 0;
}
