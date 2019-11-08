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
				stack_pop(ls);
				break;
			}
			case OP_GET_GLOBAL:
			{
				arg = codes[++pc];
				stack_push_object(ls, ls->globals_.Get(proto->constants_[arg]));
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
				int results_num = codes[++pc];
				int params_num = codes[++pc];
				Call(ls, params_num, results_num);
				break;
			}
			case OP_RETURN:
			{
				int local_var_nums = codes[++pc];
				first_result = base + local_var_nums;
				is_stop = true;
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
	int base = stack_top(ls) - param_nums; // base = func_index + 1 or first_param_index
	TObject func = stack_at(ls, base - 1);
	int first_result = 0;

	if (func.object_type_ == ObjectType_Proto)
		first_result = Execute(ls, func.value_.proto_, base);
	else if (func.object_type_ == ObjectType_CProto)
		first_result = CallCfunc(ls, func.value_.c_func_, base);
	else
		ls->error("function call not exit");

	AdjustTop(ls, first_result + result_nums); // 调整返回值数量
	
	int func_base = base - 1;
	for (int i = 0; i < result_nums; ++i) // 将 result1~resultN 移动到 func1~paramN的位置上
	{
		stack_at(ls, func_base + i) = stack_at(ls, first_result + i);
	}

	int extra = first_result - func_base; // funcBase ~ firstResult 之间func1和参数列表的总数
	stack_pop_n(ls, extra);
}

int VM::CallCfunc(LQState* ls, CFunction func, int base)
{
	CFuncStackInfo old_stack_info = ls->c_stack_info_;
	int first_result = 0;

	ls->c_stack_info_.params_num_ = stack_top(ls) - base;
	ls->c_stack_info_.base_ = base;
	ls->c_stack_info_.result_base_ = base + ls->c_stack_info_.params_num_;

	(*func)(ls);

	first_result = ls->c_stack_info_.result_base_;
	ls->c_stack_info_ = old_stack_info;

	return first_result;
}

void VM::AdjustTop(LQState* ls, int new_top) // 若实际返回值数和需要的返回值数不一致，需要增减保持一致
{
	int extra = stack_top(ls) - new_top;
	if (extra >= 0) // 丢弃多余的返回值
	{
		stack_pop_n(ls, extra);
	}
	else // 填充缺少的返回值
	{
		while (extra++)
		{
			stack_push_nil(ls);
		}
	}
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
