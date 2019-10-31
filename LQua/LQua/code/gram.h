#pragma once

#include "../base/lobject.h"
#include "../base/state.h"
#include "../base/basedef.h"
#include "lex.h"

enum VarType
{
	VarType_Global = 0, // 全局变量，如 a
	VarType_Local, // 局部变量，如 local b
	VarType_Dot, // table成员，如 a.b
	VarType_Indexed, // table成员，如 a[b]
	VarType_FuncExpr, // 表达式，若info大于0表示函数表达式，如func(...)，0表示常量
};

enum FieldType
{
	FieldType_None = 0,
	FieldType_Hash,
	FieldType_Array,
};

struct OpStack
{
public:
	OpCodePriority op_codes_[GRAM_OP_STACK_NUM];
	int top_;
};

struct LoopLabel
{
public:
	LoopLabel()
		: jump_num_(0)
		, dest_pc_(0)
		, local_num_(0)
		, pre_(nullptr)
	{
		memset(jump_pc_, 0, sizeof(jump_pc_));
	}
public:
	int jump_pc_[GRAM_LOCAL_VARS];
	int jump_num_;
	int dest_pc_;
	int local_num_;
	LoopLabel* pre_;
};

struct FuncDesc // 函数描述
{
public:
	FuncDesc()
		: proto_(nullptr)
		, pre_func_(nullptr)
		, local_value_num_(0)
		, break_info_(nullptr)
		, continue_info_(nullptr)
	{

	}
public:
	ObjectProto* proto_;
	FuncDesc* pre_func_;
	std::map<SymbolString, int> string_indexs_; // 函数内常量字符串索引
	int local_value_num_; // 函数内局部变量个数
	std::string local_vars_[GRAM_LOCAL_VARS]; // 局部变量名称
	LoopLabel* break_info_; // break、continue跳转信息
	LoopLabel* continue_info_;
};

struct VarDesc
{
public:
	VarDesc()
		: var_type_(VarType_FuncExpr)
		, info_(0)
	{

	}
	void Reset() 
	{
		var_type_ = VarType_FuncExpr;
		info_ = 0;
	}
	void AsGlobal(int const_index)
	{
		var_type_ = VarType_Global;
		info_ = const_index;
	}
	void AsLocal(int local_index)
	{
		var_type_ = VarType_Local;
		info_ = local_index;
	}
	void AsFuncExpr(int call_pc)
	{
		var_type_ = VarType_FuncExpr;
		info_ = call_pc;
	}
	void AsDot(int const_index)
	{
		var_type_ = VarType_Dot;
		info_ = const_index;
	}
	void AsIndexed()
	{
		var_type_ = VarType_Indexed;
		info_ = 0;
	}
public:
	VarType var_type_;
	int info_; // global为变量名的常量索引，local为栈位置，expr为call指令pc，dot为常量索引
};

struct ExprlistDesc // 表达式列表描述，如 1, "aaa", func()
{
public:
	ExprlistDesc()
		: expr_num_(0)
		, call_pc_(0)
	{

	}
	void Reset()
	{
		expr_num_ = 0;
		call_pc_ = 0;
	}
public:
	int expr_num_;
	int call_pc_;
};

struct TablePartDesc
{
public:
	TablePartDesc()
		: field_type_(FieldType_None)
		, fields_(0)
	{

	}
public:
	FieldType field_type_;
	int fields_;
};

class Grammer
{
public:
	Grammer()
		: lq_state_(nullptr)
		, pre_line_(0)
	{

	}
	ObjectProto* Parse(LQState* ls, const char* file_name);
private:
	void open_func(FuncDesc& fs, FuncDesc* pre_func);
	void close_func(FuncDesc& fs);
public:
	void chunk(FuncDesc& fs);
	bool stat(FuncDesc& fs);

	void name_stat(FuncDesc& fs);
	void var_or_func(FuncDesc& fs, VarDesc& var_desc);
	void var_or_func_suffix(FuncDesc& fs, VarDesc& var_desc);

	int func_call(FuncDesc& fs);
	int assignment(FuncDesc& fs, VarDesc& var_desc, int vars);
	void explist(FuncDesc& fs, ExprlistDesc& expr_desc);
	void explist1(FuncDesc& fs, ExprlistDesc& expr_desc);
	void expr(FuncDesc& fs, VarDesc& var_desc);
	void expr_code_var(FuncDesc& fs);
	void binop_expr(FuncDesc& fs, VarDesc& var_desc);
	void unary_expr(FuncDesc& fs, VarDesc& var_desc, OpStack& op_stack);
	void simple_expr(FuncDesc& fs, VarDesc& var_desc);

	void table_construct(FuncDesc& fs);
	void table_part(FuncDesc& fs, TablePartDesc& table_desc);
	int field_list(FuncDesc& fs, FieldType field_type);
	void hash_field(FuncDesc& fs);

	void if_stat(FuncDesc& fs);
	void while_stat(FuncDesc& fs);
	void break_continue_stat(FuncDesc& fs, bool is_break);
	void condition(FuncDesc& fs);
	void block(FuncDesc& fs);
	void enter_loop(FuncDesc& fs, LoopLabel& break_label, LoopLabel& continue_label);
	void level_loop(FuncDesc& fs, LoopLabel& break_label, LoopLabel& continue_label);

	void for_stat(FuncDesc& fs);
	int for_init(FuncDesc& fs);

	void local_stat(FuncDesc& fs);
	void save_localvar(FuncDesc& fs, const SymbolString& name, int var_index);

	void func_stat(FuncDesc& fs);
	void func_name(FuncDesc& fs, VarDesc& var_desc);
	void body(FuncDesc& fs);
	void parlist(FuncDesc& fs);
	void return_stat(FuncDesc& fs);
private:
	void search_var(FuncDesc& fs, VarDesc& var_desc);
	void fetch_name(FuncDesc& fs, SymbolString& symbol_str);
	int name_constant(FuncDesc& fs);
	int string_constant(FuncDesc& fs, const char* str);
	int number_constant(FuncDesc& fs, Number n);
	int next_constant(FuncDesc& fs);
private:
	void code_op(FuncDesc& fs, OpCode op);
	void code_arg(FuncDesc& fs, int arg);
	int code_op_arg(FuncDesc& fs, OpCode op, int arg); // 返回指令pc
private:
	void push_op(OpStack& op_stack, OpCodePriority priority);
	void code_higher_op(FuncDesc& fs, OpStack& op_stack, int priority);
private:
	void next_token();
	void check(TokenType token_type);
	void check_next(TokenType token_type);
	bool optional(TokenType token_type);
	void error(const char* msg);
private:
	LQState* lq_state_;
	Lex lexical_analyzer_;
	Token cur_token_;
	int pre_line_;
};