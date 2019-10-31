#pragma once

#include <cstring>
#include <string>
#include <cstdarg>
#include <vector>
#include <map>

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

	BUILT_IN_FUNC_NUM = 200,
};

enum
{
	INIT_SIZE_RUN_STACK = 1 << 8,
	INIT_SIZE_GLOBAL_VAR = 1 << 5,
	INIT_SIZE_STRING_SET = 1 << 6,
	INIT_SIZE_CONSTANT = 1 << 4,
	INIT_SIZE_CODE = 1 << 10,
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
	OP_PUSH_NIL = 1,          /* [nil数量] */
	OP_POP,                   /* [弹出数量] */
	OP_SET_GLOBAL,            /* [常量索引] */
	OP_GET_GLOBAL,            /* [常量索引] */
	OP_PUSH_CONSTANT,         /* [常量索引] */
	OP_PUSH_LOCAL,            /* [局部变量索引] */
	OP_SET_LOCAL,             /* [局部变量索引] */
	OP_NOT,                   /* [] */
	OP_MINUS,                 /* [] */
	OP_EQ,                    /* [] */
	OP_NE,                    /* [] */
	OP_GT,                    /* [] */
	OP_LT,                    /* [] */
	OP_GE,                    /* [] */
	OP_LE,                    /* [] */
	OP_CONCAT,                /* [] */
	OP_ADD,                   /* [] */
	OP_SUB,                   /* [] */
	OP_MUL,                   /* [] */
	OP_DIV,                   /* [] */
	OP_ON_TRUE_JMP,           /* [跳转偏移] */
	OP_ON_FALSE_JMP,          /* [跳转偏移] */
	OP_IF_FALSE_JMP,          /* [跳转偏移] */
	OP_JMP,                   /* [跳转偏移] */
	OP_CALL,                  /* [返回值数] [参数个数] */
	OP_RETURN,                /* [函数体活跃的局部变量总数] */
	OP_TAILCALL,              /* [函数体活跃的局部变量总数] [参数个数] */
	OP_SET_ARRAY,             /* [字段数量] */
	OP_SET_HASH,              /* [字段数量] */
	OP_NEW_TABLE,             /* [数组长度] [哈希长度] */
	OP_SET_TABLE,             /* [table/index和value中间间隔的堆栈数] */
	OP_SET_TABLE_AND_POP,     /* [] */
	OP_TABLE_DOT_GET,         /* [常量索引] */
	OP_TABLE_INDEXED_GET,     /* [] */
	OP_FOR_INIT,              /* [跳转偏移] */
	OP_FOR_LOOP,              /* [跳转偏移] */
	END_CODE,

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