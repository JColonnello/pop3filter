/* Generated by re2c 2.2 */
#include "logger.h"
#include "pop3.h"
#include <stddef.h>
#include <string.h>




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
switch (YYGETSTATE) {
default:
	goto yy0;
case 0:
	if (in->lim <= in->cur) goto yy12;
	goto yyFillLabel0;
case 1:
	if (in->lim <= in->cur) goto yy4;
	goto yyFillLabel1;
case 2:
	if (in->lim <= in->cur) goto yy7;
	goto yyFillLabel2;
case 3:
	if (in->lim <= in->cur) goto yy23;
	goto yyFillLabel3;
case 4:
	if (in->lim <= in->cur) goto yy17;
	goto yyFillLabel4;
case 5:
	if (in->lim <= in->cur) goto yy19;
	goto yyFillLabel5;
}

#undef YYGETSTATE

	for (;;)
	{
		in->tok = in->mar = in->cur;
		

yy0:
	switch (in->cond) {
	case yycbegin:
		goto yyc_begin;
	case yycline:
		goto yyc_line;
	}
/* *********************************** */
yyc_begin:
yyFillLabel0:
	yych = *in->cur;
	switch (yych) {
	case '.':	goto yy5;
	default:
		if (in->lim <= in->cur) {
			in->state = 0;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy3;
	}
yy3:
	++in->cur;
yy4:
	in->cond = yycline;
	{ COPY; continue; }
yy5:
	in->mar = ++in->cur;
yyFillLabel1:
	yych = *in->cur;
	switch (yych) {
	case '\r':	goto yy6;
	case '.':	goto yy8;
	default:
		if (in->lim <= in->cur) {
			in->state = 1;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy4;
	}
yy6:
	++in->cur;
yyFillLabel2:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy10;
	default:
		if (in->lim <= in->cur) {
			in->state = 2;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy7;
	}
yy7:
	in->cur = in->mar;
	goto yy4;
yy8:
	++in->cur;
	in->cond = yycline;
	{ in->writeBuf[in->written++] = '.'; continue; }
yy10:
	++in->cur;
	{ result = true; break; }
yy12:
	{ result = true; break; }
/* *********************************** */
yyc_line:
yyFillLabel3:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy18;
	case '\r':	goto yy20;
	default:
		if (in->lim <= in->cur) {
			in->state = 3;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy15;
	}
yy15:
	++in->cur;
yyFillLabel4:
	yych = *in->cur;
	switch (yych) {
	case '\n':
	case '\r':	goto yy17;
	default:
		if (in->lim <= in->cur) {
			in->state = 4;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy15;
	}
yy17:
	{ COPY; continue; }
yy18:
	++in->cur;
yy19:
	{ in->writeBuf[in->written++] = *in->cur; continue; }
yy20:
	++in->cur;
yyFillLabel5:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy21;
	default:
		if (in->lim <= in->cur) {
			in->state = 5;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy19;
	}
yy21:
	++in->cur;
	in->cond = yycbegin;
	{ in->writeBuf[in->written++] = '\r'; in->writeBuf[in->written++] = '\n'; continue; }
yy23:
	{ result = true; break; }

	}

	in->state = -1;
	return result;
}

bool stuffMail(Input *in)
{
	char yych;
	bool result;

#define YYGETSTATE in->state
switch (YYGETSTATE) {
default:
	goto yy24;
case 6:
	if (in->lim <= in->cur) goto yy31;
	goto yyFillLabel6;
case 7:
	if (in->lim <= in->cur) goto yy42;
	goto yyFillLabel7;
case 8:
	if (in->lim <= in->cur) goto yy36;
	goto yyFillLabel8;
case 9:
	if (in->lim <= in->cur) goto yy38;
	goto yyFillLabel9;
}

#undef YYGETSTATE

	for (;;)
	{
		in->tok = in->mar = in->cur;
		

yy24:
	switch (in->cond) {
	case yycbegin:
		goto yyc_begin;
	case yycline:
		goto yyc_line;
	}
/* *********************************** */
yyc_begin:
yyFillLabel6:
	yych = *in->cur;
	switch (yych) {
	case '.':	goto yy29;
	default:
		if (in->lim <= in->cur) {
			in->state = 6;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy27;
	}
yy27:
	++in->cur;
	in->cond = yycline;
	{ COPY; continue; }
yy29:
	++in->cur;
	in->cond = yycline;
	{ in->writeBuf[in->written++] = '.'; in->writeBuf[in->written++] = '.'; continue; }
yy31:
	{ char str[] = "\r\n.\r\n"; memcpy(&in->writeBuf[in->written], str, sizeof(str) - 1); in->written += sizeof(str) - 1; result = true; break; }
/* *********************************** */
yyc_line:
yyFillLabel7:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy37;
	case '\r':	goto yy39;
	default:
		if (in->lim <= in->cur) {
			in->state = 7;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy34;
	}
yy34:
	++in->cur;
yyFillLabel8:
	yych = *in->cur;
	switch (yych) {
	case '\n':
	case '\r':	goto yy36;
	default:
		if (in->lim <= in->cur) {
			in->state = 8;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy34;
	}
yy36:
	{ COPY; continue; }
yy37:
	++in->cur;
yy38:
	{ in->writeBuf[in->written++] = *in->cur; continue; }
yy39:
	++in->cur;
yyFillLabel9:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy40;
	default:
		if (in->lim <= in->cur) {
			in->state = 9;
			{ if(in->cond == yycline) { COPY; in->tok = in->mar = in->cur; } return false; }
		}
		goto yy38;
	}
yy40:
	++in->cur;
	in->cond = yycbegin;
	{ in->writeBuf[in->written++] = '\r'; in->writeBuf[in->written++] = '\n'; continue; }
yy42:
	{ char str[] = "\r\n.\r\n"; memcpy(&in->writeBuf[in->written], str, sizeof(str) - 1); in->written += sizeof(str) - 1; result = true; break; }

	}

	in->state = -1;
	return result;
}

bool parsePopResponse(Input *in, bool multiline)
{
	char yych;
	bool result;

#define YYGETSTATE in->state
switch (YYGETSTATE) {
default:
	goto yy43;
case 10:
	if (in->lim <= in->cur) goto yy58;
	goto yyFillLabel10;
case 11:
	if (in->lim <= in->cur) goto yy47;
	goto yyFillLabel11;
case 12:
	if (in->lim <= in->cur) goto yy47;
	goto yyFillLabel12;
case 13:
	if (in->lim <= in->cur) goto yy51;
	goto yyFillLabel13;
case 14:
	if (in->lim <= in->cur) goto yy51;
	goto yyFillLabel14;
case 15:
	if (in->lim <= in->cur) goto yy51;
	goto yyFillLabel15;
case 16:
	if (in->lim <= in->cur) goto yy69;
	goto yyFillLabel16;
case 17:
	if (in->lim <= in->cur) goto yy63;
	goto yyFillLabel17;
case 18:
	if (in->lim <= in->cur) goto yy65;
	goto yyFillLabel18;
case 19:
	if (in->lim <= in->cur) goto yy84;
	goto yyFillLabel19;
case 20:
	if (in->lim <= in->cur) goto yy74;
	goto yyFillLabel20;
case 21:
	if (in->lim <= in->cur) goto yy76;
	goto yyFillLabel21;
case 22:
	if (in->lim <= in->cur) goto yy79;
	goto yyFillLabel22;
case 23:
	if (in->lim <= in->cur) goto yy79;
	goto yyFillLabel23;
case 24:
	if (in->lim <= in->cur) goto yy79;
	goto yyFillLabel24;
}

#undef YYGETSTATE

	for (;;)
	{
		in->tok = in->mar = in->cur;
		

yy43:
	switch (in->cond) {
	case yycstatus:
		goto yyc_status;
	case yycmsg:
		goto yyc_msg;
	case yycextra:
		goto yyc_extra;
	}
/* *********************************** */
yyc_status:
yyFillLabel10:
	yych = *in->cur;
	switch (yych) {
	case '+':	goto yy48;
	case '-':	goto yy49;
	default:
		if (in->lim <= in->cur) {
			in->state = 10;
			{ return false; }
		}
		goto yy46;
	}
yy46:
	++in->cur;
yy47:
	{ char str[] = "-ERR Invalid response from origin"; memcpy(&in->writeBuf[in->written], str, sizeof(str) - 1); in->written += sizeof(str) - 1; result = true; break; }
yy48:
	in->mar = ++in->cur;
yyFillLabel11:
	yych = *in->cur;
	switch (yych) {
	case 'O':	goto yy50;
	default:
		if (in->lim <= in->cur) {
			in->state = 11;
			{ return false; }
		}
		goto yy47;
	}
yy49:
	in->mar = ++in->cur;
yyFillLabel12:
	yych = *in->cur;
	switch (yych) {
	case 'E':	goto yy52;
	default:
		if (in->lim <= in->cur) {
			in->state = 12;
			{ return false; }
		}
		goto yy47;
	}
yy50:
	++in->cur;
yyFillLabel13:
	yych = *in->cur;
	switch (yych) {
	case 'K':	goto yy53;
	default:
		if (in->lim <= in->cur) {
			in->state = 13;
			{ return false; }
		}
		goto yy51;
	}
yy51:
	in->cur = in->mar;
	goto yy47;
yy52:
	++in->cur;
yyFillLabel14:
	yych = *in->cur;
	switch (yych) {
	case 'R':	goto yy55;
	default:
		if (in->lim <= in->cur) {
			in->state = 14;
			{ return false; }
		}
		goto yy51;
	}
yy53:
	++in->cur;
	in->cond = yycmsg;
	{ COPY; continue; }
yy55:
	++in->cur;
yyFillLabel15:
	yych = *in->cur;
	switch (yych) {
	case 'R':	goto yy56;
	default:
		if (in->lim <= in->cur) {
			in->state = 15;
			{ return false; }
		}
		goto yy51;
	}
yy56:
	++in->cur;
	in->cond = yycmsg;
	{ COPY; continue; }
yy58:
	{ result = true; break; }
/* *********************************** */
yyc_msg:
yyFillLabel16:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy64;
	case '\r':	goto yy66;
	default:
		if (in->lim <= in->cur) {
			in->state = 16;
			{ return false; }
		}
		goto yy61;
	}
yy61:
	++in->cur;
yyFillLabel17:
	yych = *in->cur;
	switch (yych) {
	case '\n':
	case '\r':	goto yy63;
	default:
		if (in->lim <= in->cur) {
			in->state = 17;
			{ return false; }
		}
		goto yy61;
	}
yy63:
	{ COPY; continue; }
yy64:
	++in->cur;
yy65:
	{ COPY; continue; }
yy66:
	++in->cur;
yyFillLabel18:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy67;
	default:
		if (in->lim <= in->cur) {
			in->state = 18;
			{ return false; }
		}
		goto yy65;
	}
yy67:
	++in->cur;
	{ COPY; if(multiline) { in->cond = yycextra; continue; } else { result = true; break; } }
yy69:
	{ result = true; break; }
/* *********************************** */
yyc_extra:
yyFillLabel19:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy75;
	case '\r':	goto yy77;
	default:
		if (in->lim <= in->cur) {
			in->state = 19;
			{ return false; }
		}
		goto yy72;
	}
yy72:
	++in->cur;
yyFillLabel20:
	yych = *in->cur;
	switch (yych) {
	case '\n':
	case '\r':	goto yy74;
	default:
		if (in->lim <= in->cur) {
			in->state = 20;
			{ return false; }
		}
		goto yy72;
	}
yy74:
	{ COPY; continue; }
yy75:
	++in->cur;
yy76:
	{ COPY; continue; }
yy77:
	in->mar = ++in->cur;
yyFillLabel21:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy78;
	default:
		if (in->lim <= in->cur) {
			in->state = 21;
			{ return false; }
		}
		goto yy76;
	}
yy78:
	++in->cur;
yyFillLabel22:
	yych = *in->cur;
	switch (yych) {
	case '.':	goto yy80;
	default:
		if (in->lim <= in->cur) {
			in->state = 22;
			{ return false; }
		}
		goto yy79;
	}
yy79:
	in->cur = in->mar;
	goto yy76;
yy80:
	++in->cur;
yyFillLabel23:
	yych = *in->cur;
	switch (yych) {
	case '\r':	goto yy81;
	default:
		if (in->lim <= in->cur) {
			in->state = 23;
			{ return false; }
		}
		goto yy79;
	}
yy81:
	++in->cur;
yyFillLabel24:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy82;
	default:
		if (in->lim <= in->cur) {
			in->state = 24;
			{ return false; }
		}
		goto yy79;
	}
yy82:
	++in->cur;
	{ COPY; result = true; break; }
yy84:
	{ result = true; break; }

	}

	in->tok = in->mar = in->cur;
	in->state = -1;
	if (result)
		in->cond = yycstatus;
	return result;
}

PopCommand parsePopRequest(Input *in, char **arg, size_t *argLen, size_t *len)
{
	char yych;
	char *args = NULL, *arge = NULL, *end;
	PopCommand result;
	char *yyt1;


#define YYGETSTATE in->state
switch (YYGETSTATE) {
default:
	goto yy85;
case 25:
	if (in->lim <= in->cur) goto yy114;
	goto yyFillLabel25;
case 26:
	if (in->lim <= in->cur) goto yy89;
	goto yyFillLabel26;
case 27:
	if (in->lim <= in->cur) goto yy89;
	goto yyFillLabel27;
case 28:
	if (in->lim <= in->cur) goto yy89;
	goto yyFillLabel28;
case 29:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel29;
case 30:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel30;
case 31:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel31;
case 32:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel32;
case 33:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel33;
case 34:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel34;
case 35:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel35;
case 36:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel36;
case 37:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel37;
case 38:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel38;
case 39:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel39;
case 40:
	if (in->lim <= in->cur) goto yy95;
	goto yyFillLabel40;
}

#undef YYGETSTATE

	for (;;)
	{
		

yy85:
yyFillLabel25:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy90;
	case '\r':	goto yy91;
	case 'U':
	case 'u':	goto yy92;
	default:
		if (in->lim <= in->cur) {
			in->state = 25;
			{ return POP_INCOMPLETE; }
		}
		goto yy88;
	}
yy88:
	in->mar = ++in->cur;
yyFillLabel26:
	yych = *in->cur;
	switch (yych) {
	case 0x00:
		if (in->lim <= in->cur) {
			in->state = 26;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	case '\n':	goto yy89;
	default:	goto yy94;
	}
yy89:
	{ end = in->cur; result = POP_INVALID; break; }
yy90:
	++in->cur;
	goto yy89;
yy91:
	++in->cur;
yyFillLabel27:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy97;
	default:
		if (in->lim <= in->cur) {
			in->state = 27;
			{ return POP_INCOMPLETE; }
		}
		goto yy89;
	}
yy92:
	in->mar = ++in->cur;
yyFillLabel28:
	yych = *in->cur;
	switch (yych) {
	case 0x00:
		if (in->lim <= in->cur) {
			in->state = 28;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	case '\n':	goto yy89;
	case 'S':
	case 's':	goto yy99;
	default:	goto yy94;
	}
yy93:
	++in->cur;
yyFillLabel29:
	yych = *in->cur;
yy94:
	switch (yych) {
	case '\n':	goto yy95;
	case '\r':	goto yy96;
	default:
		if (in->lim <= in->cur) {
			in->state = 29;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	}
yy95:
	in->cur = in->mar;
	goto yy89;
yy96:
	++in->cur;
yyFillLabel30:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy97;
	default:
		if (in->lim <= in->cur) {
			in->state = 30;
			{ return POP_INCOMPLETE; }
		}
		goto yy95;
	}
yy97:
	++in->cur;
	end = in->cur;
	{ result = POP_UNKNOWN; break; }
yy99:
	++in->cur;
yyFillLabel31:
	yych = *in->cur;
	switch (yych) {
	case 0x00:
		if (in->lim <= in->cur) {
			in->state = 31;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	case 'E':
	case 'e':	goto yy100;
	default:	goto yy94;
	}
yy100:
	++in->cur;
yyFillLabel32:
	yych = *in->cur;
	switch (yych) {
	case 0x00:
		if (in->lim <= in->cur) {
			in->state = 32;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	case 'R':
	case 'r':	goto yy101;
	default:	goto yy94;
	}
yy101:
	++in->cur;
yyFillLabel33:
	yych = *in->cur;
	switch (yych) {
	case 0x00:
		if (in->lim <= in->cur) {
			in->state = 33;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	case ' ':	goto yy102;
	default:	goto yy94;
	}
yy102:
	++in->cur;
yyFillLabel34:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy105;
	case '\r':	goto yy106;
	case ' ':	goto yy102;
	default:
		if (in->lim <= in->cur) {
			in->state = 34;
			{ return POP_INCOMPLETE; }
		}
		goto yy104;
	}
yy104:
	++in->cur;
yyFillLabel35:
	yych = *in->cur;
	switch (yych) {
	case 0x00:
		if (in->lim <= in->cur) {
			in->state = 35;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	case '\r':
		yyt1 = in->cur;
		goto yy107;
	case ' ':
		yyt1 = in->cur;
		goto yy108;
	default:	goto yy94;
	}
yy105:
	++in->cur;
yyFillLabel36:
	yych = *in->cur;
	switch (yych) {
	case '\r':
		yyt1 = in->cur;
		goto yy107;
	case ' ':
		yyt1 = in->cur;
		goto yy110;
	default:
		if (in->lim <= in->cur) {
			in->state = 36;
			{ return POP_INCOMPLETE; }
		}
		goto yy95;
	}
yy106:
	++in->cur;
yyFillLabel37:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy97;
	case '\r':
		yyt1 = in->cur;
		goto yy107;
	case ' ':
		yyt1 = in->cur;
		goto yy110;
	default:
		if (in->lim <= in->cur) {
			in->state = 37;
			{ return POP_INCOMPLETE; }
		}
		goto yy95;
	}
yy107:
	++in->cur;
yyFillLabel38:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy112;
	default:
		if (in->lim <= in->cur) {
			in->state = 38;
			{ return POP_INCOMPLETE; }
		}
		goto yy95;
	}
yy108:
	++in->cur;
yyFillLabel39:
	yych = *in->cur;
	switch (yych) {
	case '\n':	goto yy95;
	case '\r':	goto yy107;
	case ' ':	goto yy108;
	default:
		if (in->lim <= in->cur) {
			in->state = 39;
			{ return POP_INCOMPLETE; }
		}
		goto yy93;
	}
yy110:
	++in->cur;
yyFillLabel40:
	yych = *in->cur;
	switch (yych) {
	case '\r':	goto yy107;
	case ' ':	goto yy110;
	default:
		if (in->lim <= in->cur) {
			in->state = 40;
			{ return POP_INCOMPLETE; }
		}
		goto yy95;
	}
yy112:
	++in->cur;
	arge = yyt1;
	args = yyt1 - 1;
	end = in->cur;
	{ result = POP_USER; break; }
yy114:
	{ end = in->cur; result = POP_INCOMPLETE; break; }

	}

	*arg = args;
	*argLen = arge - args;
	*len = end - in->tok;
	in->state = -1;
	return result;
}