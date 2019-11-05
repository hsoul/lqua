#pragma once

#include <cstring>
#include <string>
#include <cstdarg>
#include <vector>
#include <map>
#include <stack>

class LQState;

typedef double Number;
typedef int Instruction;
typedef unsigned int HashNum;
typedef unsigned int Byte;
typedef int(*CFunction)(LQState*);
typedef std::string SymbolString;

#define FUNC_NAME __FUNCTION__
#define LQ_TRY try {
#define LQ_CATCH } catch
#define MAX_VAL(a, b) ((a) > (b) ? (a) : (b))
#define MIN_VAL(a, b) ((a) < (b) ? (a) : (b))
#define MOD_VAL(a, b) ((b) == 0 ? 0 : (a) % (b))
#define BETWEN(v, minv, maxv) ((v) >= (minv) && (v) <= (maxv))
#define NUMBER_FMT "%.14g"

#define NOT_NIL(obj) (ObjectType_NIL != (obj).object_type_)

enum
{
	SHORT_STR_SIZE = 16,
	STD_BUFFER_LEN = 512,
	LEX_EOZ = -1,

	GRAM_LOCAL_VARS = 300,
	GRAM_LOOP_JMP_NUM = 30,
	GRAM_OP_STACK_NUM = 60,
	GRAM_CODE_ARG_NUM_POS = 0,

	VM_CODE_START = 1,

	BUILT_IN_FUNC_NUM = 200,
};

enum
{
	INIT_SIZE_RUN_STACK = 1 << 8,
	INIT_SIZE_GLOBAL_VAR = 1 << 5,
	INIT_SIZE_STRING_SET = 1 << 6,
	INIT_SIZE_CONSTANT = 1 << 4,
	INIT_SIZE_CODE = 1 << 10,
	INIT_SIZE_TABLE_ARRAY = 1 << 2,
	INIT_SIZE_TABLE_HASH = 1 << 3,
};

enum TokenType
{
	TK_NONE = 0,
	TK_NUMBER, /* 1.2 */
	TK_STRING, /* "abc" */
	TK_NAME, /* a */
	TK_EOF, /* 文件结束符 */

	TK_ASSIGN, /* = */
	TK_EQ, /* == */
	TK_NE, /* ~= */
	TK_POINT, /* . */
	TK_CONCAT, /* .. */
	TK_DOTS, /* ... */

	TK_ADD, /* + */
	TK_SUB_MINUS, /* - */
	TK_MUL, /* * */
	TK_DIV, /* / */

	TK_GT, /* > */
	TK_GE, /* >= */
	TK_LT, /* < */
	TK_LE, /* <= */

	TK_L_PAREN, /* ( */
	TK_R_PAREN, /* ) */
	TK_L_BRACES, /* { */
	TK_R_BRACES, /* } */
	TK_L_SQUARE, /* [ */
	TK_R_SQUARE, /* ] */
	TK_SEMICOLON, /* ; */
	TK_COMMA, /* , */

	TK_AND, /* 保留字 */
	TK_DO,
	TK_ELSE,
	TK_ELSEIF,
	TK_END,
	TK_FUNCTION,
	TK_IF,
	TK_LOCOAL,
	TK_NIL,
	TK_NOT,
	TK_OR,
	TK_RETURN,
	TK_THEN,
	TK_WHILE,
	TK_BREAK,
	TK_CONTINUE,
	TK_FOR,

	TK_MAX,
};

enum OpCode
{
	OP_NONE = 0,
	OP_PUSH_NIL = 1,          /* [1] [nil数量] */  // [参数个数] [指令解释] 
	OP_POP,                   /* [1] [弹出数量] */
	OP_SET_GLOBAL,            /* [1] [常量索引] */
	OP_GET_GLOBAL,            /* [1] [常量索引] */
	OP_PUSH_CONSTANT,         /* [1] [常量索引] */
	OP_PUSH_LOCAL,            /* [1] [局部变量索引] */
	OP_SET_LOCAL,             /* [1] [局部变量索引] */
	OP_NOT,                   /* [0] [] */
	OP_MINUS,                 /* [0] [] */
	OP_EQ,                    /* [0] [] */
	OP_NE,                    /* [0] [] */
	OP_GT,                    /* [0] [] */
	OP_LT,                    /* [0] [] */
	OP_GE,                    /* [0] [] */
	OP_LE,                    /* [0] [] */
	OP_CONCAT,                /* [0] [] */
	OP_ADD,                   /* [0] [] */
	OP_SUB,                   /* [0] [] */
	OP_MUL,                   /* [0] [] */
	OP_DIV,                   /* [0] [] */
	OP_ON_TRUE_JMP,           /* [1] [跳转偏移] */
	OP_ON_FALSE_JMP,          /* [1] [跳转偏移] */
	OP_IF_FALSE_JMP,          /* [1] [跳转偏移] */
	OP_JMP,                   /* [1] [跳转偏移] */
	OP_CALL,                  /* [2] [返回值数] [参数个数] */
	OP_RETURN,                /* [1] [函数体活跃的局部变量总数] */
	OP_TAILCALL,              /* [2] [函数体活跃的局部变量总数] [参数个数] */
	OP_SET_ARRAY,             /* [1] [字段数量] */
	OP_SET_HASH,              /* [1] [字段数量] */
	OP_NEW_TABLE,             /* [2] [数组长度] [哈希长度] */
	OP_SET_TABLE,             /* [1] [table/index和value中间间隔的堆栈数] */
	OP_SET_TABLE_AND_POP,     /* [0] [] */
	OP_TABLE_DOT_GET,         /* [1] [常量索引] */
	OP_TABLE_INDEXED_GET,     /* [0] [] */
	OP_FOR_INIT,              /* [1] [跳转偏移] */
	OP_FOR_LOOP,              /* [1] [跳转偏移] */
	END_CODE,				  /* [0] [跳转偏移] */

	OP_MAX,
};

enum ObjectType
{
	ObjectType_Unknown = 0,
	ObjectType_NIL,
	ObjectType_Number,
	ObjectType_String,
	ObjectType_Table,
	ObjectType_Proto,
	ObjectType_CProto,
};

class Token
{
public:
	Token()
	{
		Reset();
	}
	void Reset()
	{
		token_type_ = TK_NONE;
		token_str_ = 0;
		line_ = 0;
	}
public:
	TokenType token_type_;
	const char* token_str_;
	int line_;
};

struct CFuncInfo
{
public:
	CFuncInfo()
		: func_name_(nullptr)
		, func_(nullptr)
	{

	}
	CFuncInfo(const char* func_name, CFunction func)
		: func_name_(func_name)
		, func_(func)
	{

	}
public:
	const char* func_name_;
	CFunction func_;
};

struct CFuncStackInfo
{
public:
	CFuncStackInfo()
		: params_num_(0)
		, base_(0)
		, result_base_(0)
	{

	}
	CFuncStackInfo(int params_num, int base, int result_base)
		: params_num_(params_num)
		, base_(base)
		, result_base_(result_base)
	{

	}
public:
	int params_num_;
	int base_;
	int result_base_;
};

struct LineInfo
{
public:
	LineInfo()
		: line_(0)
		, pc_(0)
	{

	}
	LineInfo(int line, int pc)
		: line_(line)
		, pc_(pc)
	{

	}
public:
	int line_;
	int pc_;
};

struct ReservedToken
{
public:
	ReservedToken()
		: token_(nullptr)
		, type_(TK_NONE)
	{

	}
	ReservedToken(const char* token, TokenType type)
		: token_(token)
		, type_(type)
	{

	}
public:
	const char* token_;
	TokenType type_;
};

struct OpCodeAttr
{
public:
	OpCodeAttr()
		: name_(nullptr)
		, args_num_(0)
	{

	}
	OpCodeAttr(const char* name, int arg_num)
		: name_(name)
		, args_num_(arg_num)
	{

	}
public:
	const char* name_;
	int args_num_;
};

struct OpCodePriority
{
public:
	OpCodePriority()
		: priority_(0)
	{

	}
public:
	int priority_;
	OpCode op_code_;
};