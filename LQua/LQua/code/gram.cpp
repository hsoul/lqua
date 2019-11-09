#include "gram.h"
#include "../base/const.h"
#include "../base/dump.h"

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
	code_op(fs, END_CODE);
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
		func_stat(fs);
		break;
	case TK_RETURN:
		return_stat(fs);
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
		if (var_desc.info_ == 0)
			error("not function call");
		fix_func_returns(fs, var_desc.info_, 0);
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
			code_push_var(fs, var_desc); // 将前面的值类型生成执行指令
			var_desc.AsFuncExpr(func_call(fs));
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

/*
函数语句解析
函数调用过程 = {
	1 = 将函数proto、参数列表压栈
	2 = 生成CALL指令
	3 = 返回CALL指令pc
}

如: function add(a,b) return a+b end;  c=add(100,200);
	生成如下指令 = {
		[01] PUSH_CONSTANT   proto(add)
		[03] SET_GLOBAL      'add'
		[05] GET_GLOBAL      'add'
		[07] PUSH_NUMBER     100
		[09] PUSH_NUMBER     200
		[11] OP_CALL         1 2
		[14] SET_GLOBAL      c
	}

function add() 等同于 add = function() ...
*/
int Grammer::func_call(FuncDesc& fs)
{
	int params_num = 1;
	int result_num = 0;

	switch (cur_token_.token_type_)
	{
	case TK_L_PAREN:
	{
		next_token();
		ExprlistDesc exprlist_desc;
		explist(fs, exprlist_desc);
		check_next(TK_R_PAREN);
		params_num = exprlist_desc.expr_num_;
		if (exprlist_desc.call_pc_ > 0) // 如果 call_pc_ > 0 说明最后一个参数列表表达式为函数调用
			fix_func_returns(fs, exprlist_desc.call_pc_, 1); // 如果函数调用为函数列表最后一个参数，此函数返回值数量设置为 1
	}
	default:
		break;
	}
	code_op(fs, OP_CALL);
	code_arg(fs, result_num); // 返回值数量由接收方决定，先设置为0
	code_arg(fs, params_num);

	return fs.proto_->codes_.size() - 3;
}

/*
多变量赋值
	1 表达式列表 按照 从左到右 顺序生成指令
	2 变量列表 按照 从右到左 顺序生成指令
例如 a,b=1,2; 指令为： push 1 -> push 2 -> set b -> set a 
*/
int Grammer::assignment(FuncDesc& fs, VarDesc& var_desc, int var_nums)
{
	if (var_desc.var_type_ == VarType_Dot)
	{

	}

	if (cur_token_.token_type_ == TK_COMMA) // 多变量赋值
	{
		next_token();
		VarDesc v2;
		var_or_func(fs, v2);
		if (VarType_FuncExpr == v2.var_type_)
			error("var syntax error");
		assignment(fs, v2, var_nums + 1); // 递归进入直到 =, 顺序解析表达式列表生成 push 指令， 然后递归返回逆序生成 set 指令
	}
	else // 解析=右边的表达式列表， 生成n组指令 // 多重赋值递归到此， 此时 vars = 等号左边变量个数 
	{
		check_next(TK_ASSIGN);
		ExprlistDesc exprlist_desc;
		explist1(fs, exprlist_desc);
		adjust_multi_assign(fs, var_nums, exprlist_desc);
	}

	code_store_var(fs, var_desc);

	return 0;
}

// return 返回值列表，或func参数列表，explist可以为空列表，exprlist1不可以为空
void Grammer::explist(FuncDesc& fs, ExprlistDesc& exprlist_desc)
{
	switch (cur_token_.token_type_)
	{
	case TK_SEMICOLON: case TK_R_PAREN: case TK_EOF:
	case TK_ELSE: case TK_ELSEIF: case TK_END:
		exprlist_desc.Reset();
		break;
	default:
		explist1(fs, exprlist_desc);
		break;
	}
}

/*
explist1 -> expr {',' expr}

expr 含义：
expr 可以为全局变量、局部变量、运算符表达式
{

}
*/
void Grammer::explist1(FuncDesc& fs, ExprlistDesc& exprlist_desc)
{
	exprlist_desc.expr_num_ = 1;
	VarDesc var_desc;
	expr(fs, var_desc);

	while (TK_COMMA == cur_token_.token_type_) // 多表达式
	{
		++exprlist_desc.expr_num_;
		code_push_var(fs, var_desc); // push ',' 前一个表达式
		next_token();
		expr(fs, var_desc);
	}

	if (var_desc.var_type_ == VarType_FuncExpr) // 如果最后一个表达式为函数调用，将call_pc_返回，用于设置返回值个数
		exprlist_desc.call_pc_ = var_desc.info_;
	else
		code_push_var(fs, var_desc);
}

/*
expr -> binop_expr { [ AND | OR ] binop_expr }

and 、or 运算符执行短路计算，and 取第一个false表达式， or 取第一个true表达式
*/
void Grammer::expr(FuncDesc& fs, VarDesc& var_desc)
{
	binop_expr(fs, var_desc);

	while (cur_token_.token_type_ == TK_ADD || cur_token_.token_type_ == TK_OR)
	{
		code_push_var(fs, var_desc); // is needed ?
		int src_pc = code_op_arg(fs, TK_ADD == cur_token_.token_type_ ? OP_ON_FALSE_JMP : OP_ON_TRUE_JMP, 0);
		next_token();
		binop_expr(fs, var_desc);
		code_push_var(fs, var_desc);
		fix_jump_to_next(fs, src_pc); // 回填跳转位置
	}
}

void Grammer::expr_code_var(FuncDesc& fs)
{

}

/*
binop_expr -> unary_expr { binop unary_expr }

表达式求值，中缀表达式转后缀表达式 -> 生成指令
*/
void Grammer::binop_expr(FuncDesc& fs, VarDesc& var_desc)
{
	OpStack op_stack;
	op_stack.top_ = 0;
	unary_expr(fs, var_desc, op_stack);

	while (g_bin_ops[cur_token_.token_type_].priority_ > 0)
	{
		code_push_var(fs, var_desc); // ? is needed
		code_higher_op(fs, op_stack, g_bin_ops[cur_token_.token_type_].priority_);
		push_op(op_stack, g_bin_ops[cur_token_.token_type_]);

		next_token();
		unary_expr(fs, var_desc, op_stack);
		code_push_var(fs, var_desc);
	}

	if (op_stack.top_ > 0)
	{
		code_higher_op(fs, op_stack, 0);
	}
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
		next_token();
		break;
	}
	case TK_NIL:
	{
		next_token();
		break;
	}
	case TK_NAME:
	{
		var_or_func(fs, var_desc);
		return;
	}
	case TK_L_PAREN:
	{
		check_next(TK_R_PAREN);
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

void Grammer::save_localvar(FuncDesc& fs, const SymbolString& name, int var_index) // 每次var_index从0开始加， 实际index = local_value_num_ + var_index // 注意 ：调用后必须将
{
	if (fs.local_value_num_ + var_index + 1 > GRAM_LOCAL_VARS)
		error("too many local variables");
	fs.local_vars_[fs.local_value_num_ + var_index] = name;
}

// func_stat -> FUNCTION funcname body
void Grammer::func_stat(FuncDesc& fs)
{
	next_token();
	VarDesc var_desc;
	func_name(fs, var_desc);
	body(fs);
	code_store_var(fs, var_desc); // 1. gen variable  2. push proto  3. set variable by proto 
}

void Grammer::func_name(FuncDesc& fs, VarDesc& var_desc)
{
	search_var(fs, var_desc);
}

void Grammer::body(FuncDesc& fs)
{
	FuncDesc sub_func_desc;
	open_func(sub_func_desc, &fs);

	check_next(TK_L_PAREN); // 参数列表
	parlist(sub_func_desc);
	check_next(TK_R_PAREN);

	chunk(sub_func_desc);
	check_next(TK_END);
	close_func(sub_func_desc);

	TObject obj;
	obj.AsProto(sub_func_desc.proto_);
	fs.proto_->constants_.Add(obj);

	code_op_arg(fs, OP_PUSH_CONSTANT, fs.proto_->constants_.size() - 1);
}

void Grammer::parlist(FuncDesc& fs)
{
	int params_num = 0;
	SymbolString param_name;

	while (cur_token_.token_type_ == TK_NAME)
	{
		fetch_name(fs, param_name);
		save_localvar(fs, param_name, params_num++);
		if (!optional(TK_COMMA))
			break;
	}

	fs.local_value_num_ += params_num;
	fs.proto_->codes_[GRAM_CODE_ARG_NUM_POS] = params_num;
}

void Grammer::return_stat(FuncDesc& fs)
{
	ExprlistDesc exprlist_desc;
	
	next_token();
	explist(fs, exprlist_desc);

	if (exprlist_desc.call_pc_ > 0) // 返回值最后一个表达式为函数调用
	{
		fs.proto_->codes_[exprlist_desc.call_pc_] = OP_TAILCALL;
		fs.proto_->codes_[exprlist_desc.call_pc_ + 1] = fs.local_value_num_;
	}
	else
	{
		code_op_arg(fs, OP_RETURN, fs.local_value_num_);
	}

	optional(TK_SEMICOLON);
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

int Grammer::number_constant(FuncDesc& fs, Number n)
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

void Grammer::code_push_var(FuncDesc& fs, VarDesc& var_desc)
{
	switch (var_desc.var_type_)
	{
	case VarType_Local:
		code_op_arg(fs, OP_PUSH_LOCAL, var_desc.info_); // info_ local为栈位置
		break;
	case VarType_Global:
		code_op_arg(fs, OP_GET_GLOBAL, var_desc.info_); // info_ global为变量名的常量索引
		break;
	case VarType_Dot:
		code_op_arg(fs, OP_TABLE_DOT_GET, var_desc.info_); // info_ dot为常量索引
		break;
	case VarType_Indexed:
		code_op(fs, OP_TABLE_INDEXED_GET);
		break;
	case VarType_FuncExpr: // info_ expr为call指令pc
		fix_func_returns(fs, var_desc.info_, 1); // 函数调用作为中间表达式（expression），返回值数量强制设置为1 
		break;
	default:
		break;
	}
	var_desc.Reset();
}

void Grammer::code_store_var(FuncDesc& fs, VarDesc& var_desc)
{
	switch (var_desc.var_type_)
	{
	case VarType_Local:
		code_op_arg(fs, OP_SET_LOCAL, var_desc.info_);
		break;
	case VarType_Global:
		code_op_arg(fs, OP_SET_GLOBAL, var_desc.info_);
		break;
	default:
		break;
	}
}

void Grammer::fix_jump_to_next(FuncDesc& fs, int src_pc)
{
	fix_jump_dest(fs, src_pc, fs.proto_->codes_.size());
}

void Grammer::fix_jump_dest(FuncDesc& fs, int src_pc, int dest_pc)
{
	int offset = dest_pc - (src_pc + 2);
	fs.proto_->codes_[src_pc + 1] = offset;
}

void Grammer::fix_op_arg(FuncDesc& fs, int pc, int arg)
{
	fs.proto_->codes_[pc] = arg;
}

// 在多重赋值表达式里，函数调用若为最后一个表达式，返回值数量动态调整
void Grammer::adjust_multi_assign(FuncDesc & fs, int need_num, ExprlistDesc & exprlist_desc)
{
	int left_need = need_num - exprlist_desc.expr_num_;
	adjust_stack(fs, left_need);
}

void Grammer::adjust_stack(FuncDesc& fs, int need_num)
{
	if (need_num > 0)
		code_op_arg(fs, OP_PUSH_NIL, need_num);
	else if (need_num < 0)
		code_op_arg(fs, OP_POP, -need_num);
}

/*
函数返回值数量自适应
	f();				-- adjusted to 0
	g(x, f());			-- f() is adjusted to 1
	a, b, c = f(), x;	-- f() is adjusted to 1 result (and c gets nil)
	a, b, c = x, f();	-- f() is adjuested to 2
	a, b, c = f();		-- f() is adjusted to 3
	return f();			-- return all values returned by f()
*/
void Grammer::fix_func_returns(FuncDesc& fs, int call_pc, int result_num)
{
	if (call_pc > 0)
	{
		fs.proto_->codes_[call_pc + 1] = result_num;
	}
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
