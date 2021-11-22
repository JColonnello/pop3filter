#include "management.h"

/*!header:re2c:on*/
#pragma once
#include <management/management.h>
#include <stddef.h>
/*!types:re2c*/
RequestStatus lexRequest(const char *str, size_t len, const char **data, size_t *dataLen);
/*!header:re2c:off*/

/*!re2c
    re2c:api:style = free-form;
    re2c:define:YYCTYPE = "char";
*/

RequestStatus lexRequest(const char *str, size_t len, const char **data, size_t *dataLen)
{
	const char *YYCURSOR = str, *YYLIMIT = str + len, *YYMARKER, *arg, *end;
	RequestStatus result = ERROR;
	enum YYCONDTYPE cond = yyccmd;
	/*!stags:re2c format = 'const char *@@;\n'; */

	for (;;)
	{
		/*!re2c
		    re2c:eof = 0;
		    re2c:yyfill:enable = 0;
		    re2c:flags:tags = 1;
		    re2c:define:YYGETCONDITION = "cond";
		    re2c:define:YYSETCONDITION = "cond = @@;";

		    <cmd> 'stats'                               => done     { result = STATS; continue; }
		    <cmd> 'SET_ERROR_FILE ' @arg [^\n]* @end    => done     { result = SET_ERROR_FILE; *data = arg; *dataLen = end - arg; continue; }
		    <cmd,done> *                                            { return ERROR; }
		    <cmd,done> "\n"                                         { return result; }
		    <cmd,done> $                                            { return result; }
		*/
	}

	return ERROR;
}
