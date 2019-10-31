#pragma once

#include "basedef.h"

extern const char* g_token_names[TK_MAX]; // token 名称
extern TokenType g_char_tokens[256]; // 单字符对应的token
extern ReservedToken g_reserved[TK_MAX]; // 保留字
extern OpCodeAttr g_opcode_attrs[OP_MAX]; // opcode 参数个数
extern OpCodePriority g_unary_ops[TK_MAX]; // 一元运算符的优先级
extern OpCodePriority g_bin_ops[TK_MAX]; // 二元运算符的优先级

void InitConstants();