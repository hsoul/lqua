#include "dump.h"
#include "common.h"

static FileWriter g_logger;

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
