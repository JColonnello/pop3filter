/* Generated by re2c 2.2 */

#pragma once
#include <management/management.h>
#include <stddef.h>
enum YYCONDTYPE {
	yyccmd,
	yycdone,
};

RequestStatus lexRequest(const char *str, size_t len, const char **data, size_t *dataLen);
