#include "gram.h"
#include "../base/const.h"

ObjectProto* Grammer::Parse(LQState* ls, const char* file_name)
{
	lq_state_ = ls;
	FuncDesc fs; // 源码运行在一个隐式的函数内
	open_func(fs, nullptr);
	fs.proto_->file_name_ = file_name;

	if (lexical_analyzer_.OpenFile(file_name)) // 整个源码文件作为一个chunk处理，chunk表示代码块，等同于block。代码块可以嵌套，比如while嵌套while
	{
		try
		{
			next_token();
			chunk(fs);
		}
		catch (RuntimeError& e)
		{
			printf("============= parse exception =============\n");
			printf("[FILE] %s:%d, [ERROR] %s\n", file_name, e.line_, e.msg_.c_str());
			fs.proto_->codes_.clear();
			fs.proto_->pc_lines_.clear();
		}
	}

	lexical_analyzer_.CloseFile();
	close_func(fs);

	return fs.proto_;
}

void Grammer::open_func(FuncDesc& fs, FuncDesc* pre_func)
{
	int arg_num = 0; // proto first value is arg num
	fs.proto_ = lq_state_->NewProto(FUNC_NAME);
	fs.pre_func_ = pre_func;
	code_arg(fs, arg_num);
}

void Grammer::close_func(FuncDesc& fs)
{

}

void Grammer::chunk(FuncDesc& fs)
{
	while (stat(fs))
	{
		optional(TK_SEMICOLON);
	}
}

bool Grammer::stat(FuncDesc& fs)
{
	bool chunk_continue = true;

	switch (cur_token_.token_type_)
	{
	case TK_NAME:
		name_stat(fs);
		break;
	case TK_IF:
		break;
	case TK_WHILE:
		break;
	case TK_FOR:
		break;
	case TK_LOCOAL:
		break;
	case TK_FUNCTION:
		break;
	case TK_RETURN:
		break;
	case TK_BREAK: case TK_CONTINUE:
		break;
	case TK_DO:
		break;
	case TK_SEMICOLON: // 单独分号， 会停止stat解析； 分号只能用于语句后面
	case TK_END: // end 结束语句块
	case TK_ELSE: // 上一个if或else语句块的结束
	case TK_ELSEIF:
	case TK_EOF: // 源码块结束
		chunk_continue = false;
		break;
	default:
		error("unexpected token");
		chunk_continue = false;
		break;
	}

	return chunk_continue;
}

void Grammer::name_stat(FuncDesc& fs)
{
	VarDesc var_desc;
	var_or_func(fs, var_desc);

	if (var_desc.var_type_ == VarType_FuncExpr) // stat -> func(...)
	{

	}
	else // stat -> assignment
	{
		int left_extera = assignment(fs, var_desc, 1);
	}
}

void Grammer::var_or_func(FuncDesc& fs, VarDesc& var_desc)
{
	search_var(fs, var_desc);
	var_or_func_suffix(fs, var_desc);
}

void Grammer::var_or_func_suffix(FuncDesc& fs, VarDesc& var_desc)
{
	while (true)
	{
		switch (cur_token_.token_type_)
		{
		case TK_L_PAREN: // (
		{
			break;
		}
		case TK_L_SQUARE: // [
		{
			break;
		}
		case TK_POINT: // .
		{
			break;
		}
		default:
			return;
		}
	}
}

int Grammer::func_call(FuncDesc& fs)
{
	return 0;
}

int Grammer::assignment(FuncDesc& fs, VarDesc& var_desc, int vars)
{
	int left = 0;

	if (var_desc.var_type_ == VarType_Dot)
	{

	}

	if (cur_token_.token_type_ == TK_COMMA) // 多变量赋值
	{

	}
	else // 解析=右边的表达式列表， 生成n组指令
	{
		check_next(TK_ASSIGN);
		ExprlistDesc desc;
		explist1(fs, desc);
	}

	return 0;
}

/*
explist1 -> expr {',' expr}

expr 含义：
	expr 可以为全局变量、局部变量、运算符表达式
	{
		
	}
*/
void Grammer::explist(FuncDesc& fs, ExprlistDesc& exprlist_desc)
{
	exprlist_desc.expr_num_ = 1;
	VarDesc var_desc;
	expr(fs, var_desc);
}

void Grammer::explist1(FuncDesc& fs, ExprlistDesc& expr_desc)
{

}

/*
expr -> binop_expr { [ AND | OR ] binop_expr }

and 、or 运算符执行短路计算，and 取第一个false表达式， or 取第一个true表达式
*/
void Grammer::expr(FuncDesc& fs, VarDesc& var_desc)
{
	binop_expr(fs, var_desc);
}

void Grammer::expr_code_var(FuncDesc& fs)
{

}

/*
binop_expr -> unary_expr { binop unary_expr }

表达式求值，中缀表达式转后缀表达式
*/
void Grammer::binop_expr(FuncDesc& fs, VarDesc& var_desc)
{
	OpStack op_stack;
	op_stack.top_ = 0;
	unary_expr(fs, var_desc, op_stack);
}

/* unary_expr -> { not | '-' } simple_exp */
void Grammer::unary_expr(FuncDesc& fs, VarDesc& var_desc, OpStack& op_stack)
{
	while (g_unary_ops[cur_token_.token_type_].priority_ > 0) // 一元运算符压栈，优先级高于二元运算符
	{
		push_op(op_stack, g_unary_ops[cur_token_.token_type_]);
		next_token();
	}
	simple_expr(fs, var_desc);
}

// simple_exp -> NUMBER | STRING | NIL | NAME | '(' expr ')' | table_construct 
void Grammer::simple_expr(FuncDesc& fs, VarDesc& var_desc)
{
	switch (cur_token_.token_type_)
	{
	case TK_NUMBER: // 针对常量数字、字符串生成指令
	{
		code_op_arg(fs, OP_PUSH_CONSTANT, number_constant(fs, ::atof(cur_token_.token_str_)));
		next_token();
		break;
	}
	case TK_STRING:
	{
		break;
	}
	case TK_NIL:
	{
		break;
	}
	case TK_NAME:
	{
		break;
	}
	case TK_L_PAREN:
	{
		break;
	}
	case TK_L_BRACES:
	{
		break;
	}
	default:
		break;
	}
	var_desc.Reset();
}

void Grammer::table_construct(FuncDesc& fs)
{

}

void Grammer::table_part(FuncDesc& fs, TablePartDesc& table_desc)
{

}

int Grammer::field_list(FuncDesc& fs, FieldType field_type)
{
	return 0;
}

void Grammer::hash_field(FuncDesc& fs)
{

}

void Grammer::if_stat(FuncDesc& fs)
{

}

void Grammer::while_stat(FuncDesc& fs)
{

}

void Grammer::break_continue_stat(FuncDesc& fs, bool is_break)
{

}

void Grammer::condition(FuncDesc& fs)
{

}

void Grammer::block(FuncDesc& fs)
{

}

void Grammer::enter_loop(FuncDesc& fs, LoopLabel& break_label, LoopLabel& continue_label)
{

}

void Grammer::level_loop(FuncDesc& fs, LoopLabel& break_label, LoopLabel& continue_label)
{

}

void Grammer::for_stat(FuncDesc& fs)
{

}

int Grammer::for_init(FuncDesc& fs)
{
	return 0;
}

void Grammer::local_stat(FuncDesc& fs)
{

}

void Grammer::save_localvar(FuncDesc& fs, const SymbolString& name, int var_index)
{

}

void Grammer::func_stat(FuncDesc& fs)
{

}

void Grammer::func_name(FuncDesc& fs, VarDesc& var_desc)
{

}

void Grammer::body(FuncDesc& fs)
{

}

void Grammer::parlist(FuncDesc& fs)
{

}

void Grammer::return_stat(FuncDesc& fs)
{

}

void Grammer::search_var(FuncDesc& fs, VarDesc& var_desc)
{
	SymbolString symbol_str;
	fetch_name(fs, symbol_str);

	int local_index = -1;
	for (int i = fs.local_value_num_ - 1; i > 0; --i)
	{
		if (fs.local_vars_[i] == symbol_str) 
		{
			local_index = i;
			break;
		}
	}

	if (-1 != local_index)
		var_desc.AsLocal(local_index);
	else
		var_desc.AsGlobal(string_constant(fs, symbol_str.c_str()));
}

void Grammer::fetch_name(FuncDesc& fs, SymbolString& symbol_str)
{
	check(TK_NAME);
	symbol_str = cur_token_.token_str_;
	next_token();
}

int Grammer::name_constant(FuncDesc& fs)
{
	return 0;
}

int Grammer::string_constant(FuncDesc& fs, const char* str)
{
	SymbolString symbol_str = str;
	auto iter = fs.string_indexs_.find(symbol_str);
	if (iter != fs.string_indexs_.end())
		return iter->second;

	int index = next_constant(fs);
	fs.string_indexs_[symbol_str] = index;

	lq_state_->NewString(fs.proto_->constants_[index], str, 0, FUNC_NAME, GCState_Fixed);
	
	return index;
}

int Grammer::number_constant(FuncDesc & fs, Number n)
{
	for (int i = 0; i < fs.proto_->constants_.size(); ++i)
	{
		TObject& obj = fs.proto_->constants_[i];
		if (obj.object_type_ == ObjectType_Number && NUM_VAL(obj) == n)
			return i;
	}
	int index = next_constant(fs);
	fs.proto_->constants_[index].AsNumber(n);

	return index;
}

int Grammer::next_constant(FuncDesc & fs)
{
	fs.proto_->constants_.Add(TObject());
	return fs.proto_->constants_.size() - 1;
}

void Grammer::code_op(FuncDesc& fs, OpCode op)
{
	fs.proto_->AddLineOpCode(op, pre_line_);
}

void Grammer::code_arg(FuncDesc& fs, int arg)
{
	fs.proto_->codes_.Add(arg);
}

int Grammer::code_op_arg(FuncDesc& fs, OpCode op, int arg)
{
	fs.proto_->AddLineOpCode(op, pre_line_);
	fs.proto_->codes_.Add(arg);
	return fs.proto_->codes_.size() - 2;
}

void Grammer::push_op(OpStack& op_stack, OpCodePriority op_priority)
{
	if (op_stack.top_ >= GRAM_OP_STACK_NUM)
		error("too many operator");
	op_stack.op_codes_[op_stack.top_++] = op_priority;
}

void Grammer::code_higher_op(FuncDesc& fs, OpStack& op_stack, int priority)
{
	while (op_stack.top_ > 0 && priority <= op_stack.op_codes_[op_stack.top_ - 1].priority_) // 将中缀表达式转为后缀表达式序列，放入操作指令中
	{
		code_op(fs, op_stack.op_codes_[op_stack.top_ - 1].op_code_);
		--op_stack.top_;
	}
}

void Grammer::next_token()
{
	pre_line_ = cur_token_.line_;
	cur_token_ = lexical_analyzer_.NextToken();
}

void Grammer::check(TokenType token_type)
{
	if (cur_token_.token_type_ != token_type)
		error("not expected token");
}

void Grammer::check_next(TokenType token_type)
{
	check(token_type);
	next_token();
}

bool Grammer::optional(TokenType token_type)
{
	if (cur_token_.token_type_ == token_type)
	{
		next_token();
		return true;
	}
	return false;
}

void Grammer::error(const char* msg)
{
	lq_state_->error(msg, cur_token_.line_);
}
