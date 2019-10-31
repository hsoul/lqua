#pragma once

#include "lobject.h"
#include "memory.h"
#include "table.h"
#include "gcdata.h"
#include "ldata.h"

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
private:
	void OpenState();
	void CloseState();
public:
	static void error(std::string msg, int line = 0);
public:
	Array<TObject> run_stack_;
	Table globals_;
	HashSet<ObjectString> strings_;
	GCList<ObjectTable> tables_;
	int cur_pc_; // µ±Ç°Ö´ÐÐpc(program counter)
};