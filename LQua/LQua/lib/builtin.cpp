#include "builtin.h"
#include "../base/state.h"
#include "../base/lstring.h"

CFuncInfo BuiltIn::c_funcs_[BUILT_IN_FUNC_NUM] = {
	/*{ "tostring", BuiltIn::tostring },*/
	{ "print", BuiltIn::print },
	//{ "assert", BuiltIn::assert },
	//{ "floor", BuiltIn::floor },
	//{ "clock", BuiltIn::clock },
	//{ "getn", BuiltIn::getn },
	//{ "tnext", BuiltIn::tnext },
	//{ "tinsert", BuiltIn::tinsert },
	//{ "tremove", BuiltIn::tremove },
	//{ "tconcat", BuiltIn::tconcat },
	{ NULL, NULL },
};

void BuiltIn::OpenLibs(LQState* ls)
{
	for (int i = 0; i < BUILT_IN_FUNC_NUM; ++i)
	{
		CFuncInfo& func_info = c_funcs_[i];
		if (func_info.func_name_)
		{
			TObject key;
			TObject value;
			ls->NewString(key, func_info.func_name_, 0, FUNC_NAME, GCState_Fixed);
			value.AsCFunc(func_info.func_);
			ls->SetGlobal(key, value);
		}
	}
}

int BuiltIn::tostring(LQState* ls)
{
	return 0;
}

int BuiltIn::print(LQState* ls)
{
	int params_num = ParamNum(ls);

	for (int i = 1; i < params_num; ++i)
	{
		TObject obj = GetParam(ls, i, ObjectType_Unknown);
		if (i > 1)
		{
			printf("\t");
		}
		printf("%s", ObjectToString(ls, obj).c_str());
	}
	printf("\n");

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

TObject BuiltIn::GetParam(LQState* ls, int index, ObjectType object_type)
{
	if (index < 1 || index > ls->c_stack_info_.params_num_)
	{
		ls->error("wrong param index");
	}
	int address = ls->c_stack_info_.base_ + index - 1;
	TObject obj = stack_at(ls, address);
	if (object_type != ObjectType_Unknown && obj.object_type_ != object_type)
	{
		ls->error("c function param type wrong");
	}
	return obj;
}

int BuiltIn::ParamNum(LQState* ls)
{
	return ls->c_stack_info_.params_num_;
}

TObject BuiltIn::ObjectToString(LQState* ls, TObject obj)
{
	ShortString buff;

	switch (obj.object_type_)
	{
	case ObjectType_NIL:
		buff.SetStr("nil");
		break;
	case ObjectType_Number:
		buff.Format(NUMBER_FMT, NUM_VAL(obj));
		break;
	case ObjectType_CProto:
		buff.Format("cproto %p", obj.value_.c_func_);
		break;
	case ObjectType_String:
		return obj;
	case ObjectType_Proto:
		buff.Format("proto %p", obj.value_.proto_);
		break;
	case ObjectType_Table:
		buff.Format("table %p", obj.value_.table_);
		break;
	default:
		buff.SetStr("unknown");
		break;
	}
	TObject result;
	ls->NewString(result, buff.datas_, buff.length_, FUNC_NAME, GCState_NoMark);
	
	return result;
}
