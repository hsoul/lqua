#pragma once

#include "basedef.h"

extern const char* g_token_names[TK_MAX]; // token ����
extern TokenType g_char_tokens[256]; // ���ַ���Ӧ��token
extern ReservedToken g_reserved[TK_MAX]; // ������
extern OpCodeAttr g_opcode_attrs[OP_MAX]; // opcode ��������
extern OpCodePriority g_unary_ops[TK_MAX]; // һԪ����������ȼ�
extern OpCodePriority g_bin_ops[TK_MAX]; // ��Ԫ����������ȼ�

void InitConstants();