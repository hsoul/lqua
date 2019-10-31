#pragma once

#include "basedef.h"

class ObjectString;
class ObjectTable; 
class ObjectProto;

#define NUM_VAL(obj) (obj.value_.num_)
#define TABLE_VAL(obj) (obj.value_.table_->table_)
#define BOOL_VAL(obj) (IS_NIL(obj) ? 0 : 1)
#define IS_NIL(obj) (ObjectType_NIL == (obj).object_type_)
#define NOT_NIL(obj) (ObjectType_NIL != (obj).object_type_)
#define IS_INTEGER(obj) ((Number)(int)(obj).value_.num_ == (obj).value_.num_)

class TObject
{
public:
	TObject()
		: hash_num_(0)
		, short_str_len_(0)
		, object_type_(ObjectType_NIL)
	{
		value_.num_ = 0;
		value_.c_func_ = nullptr;
		value_.short_str_[0] = '\0';
		value_.long_str_ = nullptr;
		value_.table_ = nullptr;
		value_.proto_ = nullptr;
	}
	bool Equal(const TObject& t) const;
	const char* c_str() const;
	int StrLength() const;
	HashNum StrHash() const;
	bool IsLongStr() const { return 0 == short_str_len_ && ObjectType_String; }
	void SetShortString(const char* str, int len);
	void SetLongString(ObjectString* s) { object_type_ = ObjectType_String; value_.long_str_ = s; }
public:
	void AsNil() { object_type_ = ObjectType_NIL; }
	void AsNumber(Number n) { object_type_ = ObjectType_Number; value_.num_ = n; }
	void AsTable(ObjectTable* t) { object_type_ = ObjectType_Table; value_.table_ = t; }
	void AsProto(ObjectProto* p) { object_type_ = ObjectType_Proto; value_.proto_ = p; }
	void AsCFunc(CFunction f) { object_type_ = ObjectType_Proto; value_.c_func_ = f; }
public:
	union
	{
		Number num_;
		CFunction c_func_;
		char short_str_[SHORT_STR_SIZE];
		ObjectString* long_str_;
		ObjectTable* table_;
		ObjectProto* proto_;
	} value_;
	HashNum hash_num_;
	Byte short_str_len_;
	ObjectType object_type_;
};