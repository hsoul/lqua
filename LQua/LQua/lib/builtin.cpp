#include "builtin.h"
#include "../base/state.h"

CFuncInfo BuiltIn::c_funcs_[BUILT_IN_FUNC_NUM] = {
	{ "tostring", BuiltIn::tostring },
	{ "print", BuiltIn::print },
	{ "assert", BuiltIn::assert },
	{ "floor", BuiltIn::floor },
	{ "clock", BuiltIn::clock },
	{ "getn", BuiltIn::getn },
	{ "tnext", BuiltIn::tnext },
	{ "tinsert", BuiltIn::tinsert },
	{ "tremove", BuiltIn::tremove },
	{ "tconcat", BuiltIn::tconcat },
	{ NULL, NULL },
};

void BuiltIn::OpenLibs(LQState * ls)
{
}

int BuiltIn::tostring(LQState* ls)
{
	return 0;
}

int BuiltIn::print(LQState* ls)
{
	return 0;
}

int BuiltIn::assert(LQState* ls)
{

	return 0;
}

int BuiltIn::floor(LQState* ls) 
{
	return 0;
}

int BuiltIn::clock(LQState* ls)
{
	return 0;
}

int BuiltIn::getn(LQState* ls)
{
	return 0;
}

int BuiltIn::tnext(LQState* ls)
{

	return 0;
}

int BuiltIn::tinsert(LQState* ls)
{
	return 0;
}

int BuiltIn::tremove(LQState* ls)
{

	return 0;
}

int BuiltIn::tconcat(LQState* ls) 
{

	return 0;
}

TObject BuiltIn::GetParam(LQState* ls, int order, ObjectType object_type)
{
	return TObject();
}

int BuiltIn::ParamNum(LQState* ls)
{
	return 0;
}

TObject BuiltIn::ObjectToString(LQState* ls, TObject obj)
{
	return TObject();
}
