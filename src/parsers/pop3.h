/* Generated by re2c 2.2 */

#pragma once
#include "pop3/pop3.h"
#include <stdbool.h>
bool stuffMail(Input *in);
bool destuffMail(Input *in);
bool parsePopResponse(Input *in, bool multiline);
PopCommand parsePopRequest(Input *in, char **arg, size_t *argLen, size_t *len);

enum YYCONDTYPE {
	yycbegin,
	yycline,
	yycstatus,
	yycmsg,
	yycextra,
};