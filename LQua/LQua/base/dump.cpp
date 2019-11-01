#include "dump.h"
#include "common.h"
#include "lobject.h"
#include "ldata.h"
#include "const.h"

static FileWriter g_logger;

static void InitOffset(int depth, std::string& str)
{
	str.clear();
	for (int i = 1; i < depth; ++i)
	{
		str.append(4, ' ');
	}
	str.append(1, '\0');
}

void DumpObject(TObject* obj, int depth)
{
	switch (obj->object_type_)
	{
	case ObjectType_NIL:
		log("nil");
		break;
	case ObjectType_Number:
		log("%.2f", obj->value_.num_);
		break;
	case ObjectType_String:
		log("%s", obj->c_str());
		break;
	case ObjectType_Table:
		log("table");
		break;
	case ObjectType_Proto:
		DumpProto(obj->value_.proto_, depth);
		break;
	case ObjectType_CProto:
		log("cproto");
		break;
	default:
		break;
	}
}

void DumpProto(ObjectProto* proto, int depth)
{
	std::string offset_str;
	InitOffset(depth, offset_str);

	log("\n%s==================== CONSTANTS ====================\n", offset_str.c_str());
	for (int i = 0; i < proto->constants_.size(); ++i)
	{
		TObject& data = proto->constants_[i];
		switch (data.object_type_)
		{
		case ObjectType_String:
			log("%s[%d] %s\n", offset_str.c_str(), i, data.c_str());
			break;
		case ObjectType_Number:
			log("%s[%d] " NUMBER_FMT "\n", offset_str.c_str(), i, data.value_.num_);
			break;
		case ObjectType_Proto:
			log("%s[%d] PROTO\n", offset_str.c_str(), i);
			DumpObject(&data, depth + 1);
			break;
		case ObjectType_Table:
			log("table");
			break;
		default:
			break;
		}
	}

	log("\n%s==================== CODES ====================\n", offset_str.c_str());
	log("%s[%03d] arg_num = %d\n", offset_str.c_str(), GRAM_CODE_ARG_NUM_POS, proto->codes_[GRAM_CODE_ARG_NUM_POS]);
	for (int i = VM_CODE_START; i < proto->codes_.size();)
	{
		OpCodeAttr& attr = g_opcode_attrs[proto->codes_[i]];
		log("%s[%03d] %s", offset_str.c_str(), i, attr.name_);

		for (int k = 1; k <= attr.args_num_; ++k)
		{
			log("\t\t%d", proto->codes_[i + k]);
		}
		log("\n");

		i += 1 + attr.args_num_;
	}
}

void DumpGlobal(LQState* ls)
{
	log("\n==================== GLOBAL VARS ====================\n");
	TObject key;
	TObject value;
	int order = 0;
	while (true)
	{
		// key = ls->globals_.next
	}
}

void log(const char * fmt, ...)
{
	static char buff[2048] = { 0 };

	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buff, sizeof(buff) - 1, fmt, ap);
	va_end(ap);

	if (!g_logger.file_)
		g_logger.Open("debug.log");
	g_logger.Write(buff, ::strlen(buff));
}
