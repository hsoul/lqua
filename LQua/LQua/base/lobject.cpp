#include "lobject.h"
#include "ldata.h"
#include "common.h"

bool TObject::Equal(const TObject& t) const
{
	if (object_type_ != t.object_type_)
		return false;
	switch (object_type_)
	{
	case ObjectType_NIL:
		return true;
	case ObjectType_Number:
		return value_.num_ == t.value_.num_;
	case ObjectType_String:
	{
		if (short_str_len_ > 0 && t.short_str_len_ > 0)
			return hash_num_ == t.hash_num_ && short_str_len_ == t.short_str_len_ && ::strncmp(value_.short_str_, t.value_.short_str_, SHORT_STR_SIZE) == 0;
		else if (0 == short_str_len_ && t.short_str_len_ == 0)
			return value_.long_str_->Equal(*t.value_.long_str_);
		return false;
	}
	case ObjectType_Table:
		return value_.table_ == t.value_.table_;
	case ObjectType_Proto:
		return value_.proto_ == t.value_.proto_;
	case ObjectType_CProto:
		return value_.c_func_ == t.value_.c_func_;
	default:
		return false;
	}
	return false;
}

const char* TObject::c_str() const
{
	return short_str_len_ > 0 ? value_.short_str_ : value_.long_str_->str_;
}

int TObject::StrLength() const
{
	return short_str_len_ > 0 ? short_str_len_: value_.long_str_->length_;
}

HashNum TObject::StrHash() const
{
	return short_str_len_ > 0 ? hash_num_ : value_.long_str_->hash_num_;
}

void TObject::SetShortString(const char* str, int len)
{
	short_str_len_ = MIN_VAL(len, SHORT_STR_SIZE);
	for (Byte i = 0; i < short_str_len_; ++i)
	{
		value_.short_str_[i] = str[i];
	}
	value_.short_str_[short_str_len_] = '\0';
	object_type_ = ObjectType_String;
	hash_num_ = BaseFunc::GenStringHash(value_.short_str_, short_str_len_);
}
