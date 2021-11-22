#include "logger.h"

LOG_LEVEL current_level = LOG_ERROR;

void setLogLevel(LOG_LEVEL newLevel)
{
	if (newLevel >= LOG_DEBUG && newLevel <= LOG_FATAL)
		current_level = newLevel;
}

char *levelDescription(LOG_LEVEL level)
{
	static char *description[] = {"DEBUG", "INFO", "ERROR", "FATAL"};
	if (level < LOG_DEBUG || level > LOG_FATAL)
		return "";
	return description[level];
}
