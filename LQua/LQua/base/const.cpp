#include "const.h"

#define NAME(n) #n

TokenType g_char_tokens[256];
OpCodePriority g_unary_ops[TK_MAX];
OpCodePriority g_bin_ops[TK_MAX];

const char* g_token_names[TK_MAX] = {
	NAME(TK_NONE),
	NAME(TK_NUMBER),
	NAME(TK_STRING),
	NAME(TK_NAME),
	NAME(TK_EOF),
	NAME(TK_ASSIGN),
	NAME(TK_EQ),
	NAME(TK_NE),
	NAME(TK_POINT),
	NAME(TK_CONCAT),
	NAME(TK_DOTS),
	NAME(TK_ADD),
	NAME(TK_SUB_MINUS),
	NAME(TK_MUL),
	NAME(TK_DIV),
	NAME(TK_GT),
	NAME(TK_GE),
	NAME(TK_LT),
	NAME(TK_LE),
	NAME(TK_L_PAREN),
	NAME(TK_R_PAREN),
	NAME(TK_L_BRACES),
	NAME(TK_R_BRACES),
	NAME(TK_L_SQUARE),
	NAME(TK_R_SQUARE),
	NAME(TK_SEMICOLON),
	NAME(TK_COMMA),
	NAME(TK_AND),
	NAME(TK_DO),
	NAME(TK_ELSE),
	NAME(TK_ELSEIF),
	NAME(TK_END),
	NAME(TK_FUNCTION),
	NAME(TK_IF),
	NAME(TK_LOCOAL),
	NAME(TK_NIL),
	NAME(TK_NOT),
	NAME(TK_OR),
	NAME(TK_RETURN),
	NAME(TK_THEN),
	NAME(TK_WHILE),
	NAME(TK_BREAK),
	NAME(TK_CONTINUE),
	NAME(TK_FOR),
};

ReservedToken g_reserved[TK_MAX] = {
	{ "add", TK_AND },
	{ "do", TK_DO },
	{ "if", TK_IF },
	{ "else", TK_ELSE },
	{ "elseif", TK_ELSEIF },
	{ "end", TK_END },
	{ "function", TK_FUNCTION },
	{ "local", TK_LOCOAL },
	{ "nil", TK_NIL },
	{ "not", TK_NOT },
	{ "or", TK_OR },
	{ "return", TK_RETURN },
	{ "then", TK_THEN },
	{ "while", TK_WHILE },
	{ "break", TK_BREAK },
	{ "continue", TK_CONTINUE },
	{ "for", TK_FOR },
	{ nullptr, TK_NONE },
};

OpCodeAttr g_opcode_attrs[OP_MAX] = {
	{ "", 0 },
	{ NAME(OP_PUSH_NIL), 1 },
	{ NAME(OP_POP), 1 },
	{ NAME(OP_SET_GLOBAL), 1 },
	{ NAME(OP_GET_GLOBAL), 1 },
	{ NAME(OP_PUSH_CONSTANT), 1 },
	{ NAME(OP_PUSH_LOCAL), 1 },
	{ NAME(OP_SET_LOCAL), 1 },
	{ NAME(OP_NOT), 0 },
	{ NAME(OP_MINUS), 0 },
	{ NAME(OP_EQ), 0 },
	{ NAME(OP_NE), 0 },
	{ NAME(OP_GT), 0 },
	{ NAME(OP_LT), 0 },
	{ NAME(OP_GE), 0 },
	{ NAME(OP_LE), 0 },
	{ NAME(OP_CONCAT), 0 },
	{ NAME(OP_ADD), 0 },
	{ NAME(OP_SUB), 0 },
	{ NAME(OP_MUL), 0 },
	{ NAME(OP_DIV), 0 },
	{ NAME(OP_ON_TRUE_JMP), 1 },
	{ NAME(OP_ON_FALSE_JMP), 1 },
	{ NAME(OP_IF_FALSE_JMP), 1 },
	{ NAME(OP_JMP), 1 },
	{ NAME(OP_CALL), 2 },
	{ NAME(OP_RETURN), 1 },
	{ NAME(OP_TAILCALL), 2 },
	{ NAME(OP_SET_ARRAY), 1 },
	{ NAME(OP_SET_HASH), 1 },
	{ NAME(OP_NEW_TABLE), 2 },
	{ NAME(OP_SET_TABLE), 1 },
	{ NAME(OP_SET_TABLE_AND_POP), 0 },
	{ NAME(OP_TABLE_DOT_GET), 1 },
	{ NAME(OP_TABLE_INDEXED_GET), 0 },
	{ NAME(OP_FOR_INIT), 1 },
	{ NAME(OP_FOR_LOOP), 1 },
	{ NAME(END_CODE), 0 },
};

static void InitOpCodePriority(OpCodePriority* ops, TokenType token_type, int priority, OpCode opcode)
{
	ops[token_type].priority_ = priority;
	ops[token_type].op_code_ = opcode;
}

void InitConstants()
{
	::memset(g_char_tokens, 0, sizeof(g_char_tokens));
	g_char_tokens['+'] = TK_ADD;
	g_char_tokens['-'] = TK_SUB_MINUS;
	g_char_tokens['*'] = TK_MUL;
	g_char_tokens['/'] = TK_DIV;
	g_char_tokens['('] = TK_L_PAREN;
	g_char_tokens[')'] = TK_R_PAREN;
	g_char_tokens['['] = TK_L_SQUARE;
	g_char_tokens[']'] = TK_R_SQUARE;
	g_char_tokens['{'] = TK_L_BRACES;
	g_char_tokens['}'] = TK_R_BRACES;
	g_char_tokens[';'] = TK_SEMICOLON;
	g_char_tokens[','] = TK_COMMA;
	g_char_tokens['='] = TK_ASSIGN;
	g_char_tokens['>'] = TK_GT;
	g_char_tokens['<'] = TK_LT;

	::memset(g_unary_ops, 0, sizeof(g_unary_ops));
	::memset(g_bin_ops, 0, sizeof(g_bin_ops));
	InitOpCodePriority(g_bin_ops, TK_EQ, 1, OP_EQ); // ==
	InitOpCodePriority(g_bin_ops, TK_NE, 1, OP_NE); // ~=
	InitOpCodePriority(g_bin_ops, TK_GT, 1, OP_GT); // >
	InitOpCodePriority(g_bin_ops, TK_LT, 1, OP_LT); // <
	InitOpCodePriority(g_bin_ops, TK_GE, 1, OP_GE); // >=
	InitOpCodePriority(g_bin_ops, TK_LE, 1, OP_LE); // <=
	InitOpCodePriority(g_bin_ops, TK_LE, 1, OP_LE); // <=
	InitOpCodePriority(g_bin_ops, TK_CONCAT, 2, OP_CONCAT); // ..
	InitOpCodePriority(g_bin_ops, TK_ADD, 3, OP_ADD); // +
	InitOpCodePriority(g_bin_ops, TK_SUB_MINUS, 3, OP_SUB); // - ¼õºÅ
	InitOpCodePriority(g_bin_ops, TK_MUL, 4, OP_MUL); // *
	InitOpCodePriority(g_bin_ops, TK_DIV, 4, OP_DIV); // /
	InitOpCodePriority(g_unary_ops, TK_NOT, 5, OP_NOT); // not
	InitOpCodePriority(g_unary_ops, TK_SUB_MINUS, 5, OP_MINUS); // - ¸ººÅ
}
