#pragma once

#include "lobject.h"
#include "memory.h"
#include "table.h"
#include "gcdata.h"
#include "ldata.h"

#define stack_top(s)					(s->run_stack_.size())
#define stack_at(s, index)				(s->run_stack_[index])
#define stack_reverse(s, index)			(s->run_stack_[s->run_stack_.size_ + (index)])
#define stack_back(s)					(s->run_stack_[s->run_stack_.size_ - 1])
#define stack_pop(s)					(--s->run_stack_.size_)
#define stack_pop_n(s, n)				(s->run_stack_.size_ -= n)

#define stack_push_object(s, obj)		{ array_grow(s->run_stack_); s->run_stack_[s->run_stack_.size_++] = obj; }
#define stack_push_number(s, num)		{ array_grow(s->run_stack_); s->run_stack_[s->run_stack_.size_].object_type_ = ObjectType_Number; s->run_stack_[s->run_stack_.size_++].value_.num_ = num; }
#define stack_push_nil(s)				{ array_grow(s->run_stack_); s->run_stack_[s->run_stack_.size_++].object_type_ = ObjectType_NIL; }

class LQState
{
public:
	LQState();
	~LQState();
public:
	void ExecuteFile(const char* file_name);
	void ExecuteFileWithDebug(const char* file_name);
public:
	void NewString(TObject& obj, const char* str, int length, const char* pre_func, GCState state);
	ObjectProto* NewProto(const char* pre_func);
	ObjectTable* NewTable(int array_size, int hash_size, const char* pre_func, GCState gc_state);
public:
	void SetGlobal(const TObject& key, const TObject& value);
private:
	void OpenState();
	void CloseState();
public:
	static void error(std::string msg, int line = 0);
public:
	Array<TObject> run_stack_; // 运行栈
	Table globals_; // 全局变量
	HashSet<ObjectString> strings_;
	GCList<ObjectTable> tables_;
	CFuncStackInfo c_func_info_; // c 函数调用信息
	int cur_pc_; // 当前执行pc(program counter)
};