#pragma once

#include "basedef.h"
#include "table.h"
#include "lobject.h"
#include "gcdata.h"

class ObjectString
{
public:
	ObjectString()
		: str_(nullptr)
		, length_(0)
		, hash_num_(0)
	{

	}
	~ObjectString()
	{

	}
	bool Equal(const ObjectString& other) const
	{
		return hash_num_ == other.hash_num_ && length_ == other.length_ && ::strncmp(str_, other.str_, length_) == 0;
	}
public:
	char* str_;
	int length_;
	HashNum hash_num_;
	GCNode<ObjectString> gc_;
};

class ObjectProto // �洢�������󣬰���ָ���б�ͳ����б�
{
public:
	ObjectProto()
	{
		constants_.Init(INIT_SIZE_CONSTANT, FUNC_NAME);
		codes_.Init(INIT_SIZE_CODE, FUNC_NAME);
		pc_lines_.Init(INIT_SIZE_CODE, FUNC_NAME);
	}
	void AddLineOpCode(OpCode opcode, int line)
	{
		pc_lines_.Add(LineInfo(line, codes_.size()));
		codes_.Add(opcode);
	}
public:
	std::string file_name_;
	Array<TObject> constants_; // �����б�
	Array<Instruction> codes_; // ָ������
	Array<LineInfo> pc_lines_; // ָ������Ϣ
	GCNode<ObjectProto> gc_;
};

class ObjectTable
{
public:
	Table table_;
	GCNode<ObjectTable> gc_;
};