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
		    <cmd> 'GET_ERROR_FILE'						=> done     { result = GET_ERROR_FILE; continue; }
		    <cmd> 'SET_PROXY_ADDR ' @arg [^\n]* @end	=> done     { result = SET_PROXY_ADDR; *data = arg; *dataLen = end - arg; continue; }
		    <cmd> 'GET_PROXY_ADDR' 						=> done     { result = GET_PROXY_ADDR; continue; }
		    <cmd> 'SET_LISTEN_PORT ' @arg [^\n]* @end	=> done     { result = SET_LISTEN_PORT; *data = arg; *dataLen = end - arg; continue; }
		    <cmd> 'GET_LISTEN_PORT' 					=> done     { result = GET_LISTEN_PORT; continue; }
		    <cmd> 'SET_ORIGIN_PORT ' @arg [^\n]* @end	=> done     { result = SET_ORIGIN_PORT; *data = arg; *dataLen = end - arg; continue; }
		    <cmd> 'GET_ORIGIN_PORT' 					=> done     { result = GET_ORIGIN_PORT; continue; }
		    <cmd> 'SET_FILTER ' @arg [^\n]* @end		=> done     { result = SET_FILTER; *data = arg; *dataLen = end - arg; continue; }
		    <cmd> 'SET_MGMT_ADDR ' @arg [^\n]* @end		=> done     { result = SET_MGMT_ADDR; *data = arg; *dataLen = end - arg; continue; }
		    <cmd> 'GET_MGMT_ADDR' 						=> done     { result = GET_MGMT_ADDR; continue; }
		    <cmd> 'SET_MGMT_PORT ' @arg [^\n]* @end		=> done     { result = SET_MGMT_PORT; *data = arg; *dataLen = end - arg; continue; }
		    <cmd> 'GET_MGMT_PORT ' 						=> done     { result = GET_MGMT_PORT; continue; }
			<cmd,done> $											{ return result; }
			<done> '\n'												{ return result; }
			<cmd,done> *											{ return ERROR; }
		*/
		return ERROR;
	}
}
