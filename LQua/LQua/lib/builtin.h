#pragma once

#ifndef builtin_h__
#define builtin_h__

#include "../base/basedef.h"
#include "../base/lobject.h"

class LQState;

#define BUILT_IN_FUNC(func) static int func(LQState* ls);

class BuiltIn
{
public:
	static void OpenLibs(LQState* ls);
private:
	BUILT_IN_FUNC(tostring)
	BUILT_IN_FUNC(print)
	BUILT_IN_FUNC(assert)
	BUILT_IN_FUNC(floor)
	BUILT_IN_FUNC(clock)
	BUILT_IN_FUNC(getn)
	BUILT_IN_FUNC(tnext)
	BUILT_IN_FUNC(tinsert)
	BUILT_IN_FUNC(tremove)
	BUILT_IN_FUNC(tconcat)
private:
	static TObject GetParam(LQState* ls, int index, ObjectType object_type);
	static int ParamNum(LQState* ls);
	static TObject ObjectToString(LQState* ls, TObject obj);
private:
	static CFuncInfo c_funcs_[BUILT_IN_FUNC_NUM];
};

#endif // builtin_h__