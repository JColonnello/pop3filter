#include "logger.h"
#include "pop3.h"
#include <stdbool.h>
#include <string.h>
/*!header:re2c:on*/
#pragma once
#include <stddef.h>
typedef struct
{
	char *lim, *cur, *mar, *tok;
	char *buf, *writeBuf;
	int state, cond;
	size_t written, bufSize;
	/*!stags:re2c format = 'char *@@;\n'; */
} Input;

#include "pop3/pop3.h"
bool stuffMail(Input *in);
bool destuffMail(Input *in);
bool parsePopResponse(Input *in, bool multiline, bool *success);
PopCommand parsePopRequest(Input *in, char **arg, size_t *argLen, size_t *len);
/*!header:re2c:off*/

/*!re2c
    re2c:api:style = free-form;
    re2c:define:YYCTYPE = "char";
*/
#define COPY                                                                                                           \
	do                                                                                                                 \
	{                                                                                                                  \
		memcpy(&in->writeBuf[in->written], in->tok, in->cur - in->tok);                                                \
		in->written += in->cur - in->tok;                                                                              \
	} while (0)

bool destuffMail(Input *in)
{
	char yych;
	bool result;

#define YYGETSTATE in->state
/*!getstate:re2c:destuff*/
#undef YYGETSTATE

	for (;;)
	{
		in->tok = in->mar = in->cur;
		/*!local:re2c:destuff
        re2c:eof = 0;
        re2c:define:YYCURSOR   = "in->cur";
        re2c:define:YYMARKER   = "in->mar";
        re2c:define:YYLIMIT    = "in->lim";
        re2c:define:YYSETSTATE = "in->state = @@;";
        re2c:define:YYGETCONDITION = "in->cond";
        re2c:define:YYSETCONDITION = "in->cond = @@;";
        re2c:define:YYFILL     = "{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }";

        <begin> '..'        => line     { in->writeBuf[in->written++] = '.'; continue; }
        <begin> '.\r\n'                 { result = true; break; }
        <begin> *           => line     { COPY; continue; }
        <begin,line> '\r\n' => begin    { COPY; continue; }
        <line> [^\r\n]+                 { COPY; continue; }
        <line> [\r\n]                   { COPY; continue; }
        <begin,line> $                  { result = true; break; }
    */
	}

	in->state = -1;
	return result;
}

bool stuffMail(Input *in)
{
	char yych;
	bool result;

#define YYGETSTATE in->state
/*!getstate:re2c:stuff*/
#undef YYGETSTATE

	for (;;)
	{
		in->tok = in->mar = in->cur;
		/*!local:re2c:stuff
        re2c:eof = 0;
        re2c:define:YYCURSOR   = "in->cur";
        re2c:define:YYMARKER   = "in->mar";
        re2c:define:YYLIMIT    = "in->lim";
        re2c:define:YYSETSTATE = "in->state = @@;";
        re2c:define:YYGETCONDITION = "in->cond";
        re2c:define:YYSETCONDITION = "in->cond = @@;";
        re2c:define:YYFILL     = "{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }";

        <begin> '.'         => line     { in->writeBuf[in->written++] = '.'; in->writeBuf[in->written++] = '.'; continue; }
        <begin> *           => line     { COPY; continue; }
        <begin,line> '\r\n' => begin    { COPY; continue; }
        <line> [^\r\n]+                 { COPY; continue; }
        <line> [\r\n]                   { COPY; continue; }
        <begin,line> $                  { char str[] = ".\r\n"; memcpy(&in->writeBuf[in->written], str, sizeof(str) - 1); in->written += sizeof(str) - 1; result = true; break; }
    */
	}

	in->state = -1;
	return result;
}

bool parsePopResponse(Input *in, bool multiline, bool *success)
{
	char yych;
	bool result;

#define YYGETSTATE in->state
/*!getstate:re2c:response*/
#undef YYGETSTATE

	for (;;)
	{
		in->tok = in->mar = in->cur;
		/*!local:re2c:response
        re2c:eof = 0;
        re2c:define:YYCURSOR   = "in->cur";
        re2c:define:YYMARKER   = "in->mar";
        re2c:define:YYLIMIT    = "in->lim";
        re2c:define:YYSETSTATE = "in->state = @@;";
        re2c:define:YYGETCONDITION = "in->cond";
        re2c:define:YYSETCONDITION = "in->cond = @@;";
        re2c:define:YYFILL     = "{ return false; }";

        <status>            "+OK"       => msg_ok   { COPY; *success = true; continue; }
        <status>            "-ERR"      => msg_err  { COPY; *success = false; continue; }
        <msg_ok,msg_err>    [^\r\n]+                { COPY; continue; }
        <msg_ok,msg_err>    [\r\n]                  { COPY; continue; }
        <msg_ok>            '\r\n'                  { COPY; if(multiline) { in->cond = yycextra; continue; } else { result = true; break; } }
        <msg_err>           '\r\n'                  { COPY; result = true; break; }
        <extra>             [\r\n]                  { COPY; continue; }
        <extra>             '\r\n.\r\n'             { COPY; result = true; break; }
        <extra>             [^\r\n]+                { COPY; continue; }
        <*>                 $                       { result = true; break; }
        <*>                 *                       { char str[] = "-ERR Invalid response from origin\r\n"; memcpy(&in->writeBuf[in->written], str, sizeof(str) - 1); in->written += sizeof(str) - 1; result = true; break; }
    */
	}

	*success = in->cond == yycmsg_ok || in->cond == yycextra;
	in->tok = in->mar = in->cur;
	in->state = -1;
	in->cond = yycstatus;
	return result;
}

PopCommand parsePopRequest(Input *in, char **arg, size_t *argLen, size_t *len)
{
	char yych;
	char *args = NULL, *arge = NULL, *end;
	PopCommand result;
	/*!stags:re2c:request format = '#define @@ in->@@\n'; */

#define YYGETSTATE in->state
/*!getstate:re2c:request*/
#undef YYGETSTATE

	for (;;)
	{
		/*!local:re2c:request
        re2c:flags:tags = 1;
        re2c:eof = 0;
        re2c:define:YYCURSOR   = "in->cur";
        re2c:define:YYMARKER   = "in->mar";
        re2c:define:YYLIMIT    = "in->lim";
        re2c:define:YYSETSTATE = "in->state = @@;";
        re2c:define:YYGETCONDITION = "in->cond";
        re2c:define:YYSETCONDITION = "in->cond = @@;";
        re2c:define:YYFILL     = "{ return POP_INCOMPLETE; }";

        print = [\x20-\x7f];
        arg = (print\[ ])+;

        'user' [ ]+ @args arg @arge [ ]* '\r\n'         { result = POP_USER; break; }
        'pass' [ ]+ arg [ ]* '\r\n'                     { result = POP_PASS; break; }
        'quit' [ ]* '\r\n'                              { result = POP_QUIT; break; }
        'stat' [ ]* '\r\n'                              { result = POP_STAT; break; }
        'list' [ ]* '\r\n'                              { result = POP_LIST; break; }
        'list' [ ]+ arg [ ]* '\r\n'                     { result = POP_LIST_MSG; break; }
        'retr' [ ]+ arg [ ]* '\r\n'                     { result = POP_RETR; break; }
        'dele' [ ]+ arg [ ]* '\r\n'                     { result = POP_DELE; break; }
        'noop' [ ]* '\r\n'                              { result = POP_NOOP; break; }
        'rset' [ ]* '\r\n'                              { result = POP_RSET; break; }
        'capa' [ ]* '\r\n'                              { result = POP_CAPA; break; }
        
        print+ '\r\n'                                   { result = POP_UNKNOWN; break; }
        '\r\n'                                          { result = POP_INVALID; break; }
        *                                               { result = POP_INVALID; break; }
        $                                               { result = POP_INCOMPLETE; break; }
    */
	}

	end = in->cur;
	*arg = args;
	*argLen = arge - args;
	*len = end - in->tok;
	in->state = -1;
	return result;
}