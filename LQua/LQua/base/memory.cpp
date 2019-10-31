#include "memory.h"
#include <map>
#include "dump.h"

struct BlockDesc
{
	const char* func_;
	const char* pre_func_;
	int size_;
};

static std::map<void*, BlockDesc> g_blocks;

void BlockMonitor::Reset()
{
	g_blocks.clear();
}

void  BlockMonitor::Add(void* ptr, int size, const char* func, const char* pre_func)
{
	g_blocks[ptr] = { func, pre_func, size };
}

void BlockMonitor::Del(void* ptr)
{
	g_blocks.erase(ptr);
}

void  BlockMonitor::Dump()
{
	log("\n============= block stat =============\n");
	int i = 1;
	for (const auto& iter : g_blocks)
	{
		const BlockDesc& desc = iter.second;
		log("[%002d] %s -> %s %d\n", i++, desc.pre_func_, desc.func_, desc.size_);
	}
}
