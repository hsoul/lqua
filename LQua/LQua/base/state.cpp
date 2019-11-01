#include "state.h"
#include "../code/gram.h"
#include "../vm/vm.h"
#include "../base/dump.h"

LQState::LQState()
{
	run_stack_.Init(INIT_SIZE_RUN_STACK, FUNC_NAME);
	globals_.Init(0, INIT_SIZE_GLOBAL_VAR);
	strings_.Init(INIT_SIZE_STRING_SET);
	OpenState();
	cur_pc_ = 0;
}

LQState::~LQState()
{
	CloseState();
}

void LQState::ExecuteFile(const char* file_name)
{
	Grammer gram;
	ObjectProto* proto = gram.Parse(this, file_name);
	DumpProto(proto, 0);
	VM::Run(this, proto);
}

void LQState::ExecuteFileWithDebug(const char* file_name)
{

}

void LQState::NewString(TObject& obj, const char* str, int length, const char* pre_func, GCState state)
{
	length = length == 0 ? ::strlen(str) : length;
	if (length < SHORT_STR_SIZE) // 短字符串
	{
		obj.SetShortString(str, length);
		return;
	}
	ObjectString temp_string;
	temp_string.str_ = (char*)str;
	temp_string.length_ = length;
	temp_string.hash_num_ = BaseFunc::GenStringHash(str, length);

	ObjectString* obj_str = strings_.Find(temp_string);
	temp_string.str_ = nullptr;
	if (obj_str) // 如果已经存在, 复用
	{
		obj.SetLongString(obj_str);
		return;
	}

	obj_str = Allocator<ObjectString>::New(FUNC_NAME, pre_func); // 新字符串
	obj_str->str_ = Allocator<char>::NewArray(length + 1, FUNC_NAME, pre_func);
	::memcpy(obj_str->str_, str, length);
	obj_str->length_ = length;
	obj_str->hash_num_ = temp_string.hash_num_;
	obj_str->gc_.state_ = state;
	strings_.Add(obj_str);
	obj.SetLongString(obj_str);
}

ObjectProto* LQState::NewProto(const char* pre_func)
{
	return Allocator<ObjectProto>::New(FUNC_NAME, pre_func);
}

ObjectTable* LQState::NewTable(int array_size, int hash_size, const char* pre_func, GCState gc_state)
{
	ObjectTable* obj = Allocator<ObjectTable>::New(FUNC_NAME, pre_func);
	obj->table_.Init(array_size, hash_size);
	obj->gc_.state_ = gc_state;
	tables_.InsertHead(obj);
	return obj;
}

void LQState::OpenState()
{

}

void LQState::CloseState()
{

}

void LQState::error(std::string msg, int line)
{
	throw RuntimeError(msg, line);
}