/* Generated by re2c 2.2 */
#include "management.h"





RequestStatus lexRequest(const char *str, size_t len, const char **data, size_t *dataLen)
{
	const char *YYCURSOR = str, *YYLIMIT = str + len, *YYMARKER, *arg, *end;
	RequestStatus result = ERROR;
	enum YYCONDTYPE cond = yyccmd;
	const char *yyt1;


	for (;;)
	{
		
{
	char yych;
	switch (cond) {
	case yyccmd:
		goto yyc_cmd;
	case yycdone:
		goto yyc_done;
	}
/* *********************************** */
yyc_cmd:
	yych = *YYCURSOR;
	switch (yych) {
	case 'G':
	case 'g':	goto yy4;
	case 'S':
	case 's':	goto yy5;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy169;
		goto yy2;
	}
yy2:
	++YYCURSOR;
yy3:
	{ return ERROR; }
yy4:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
	case 'E':
	case 'e':	goto yy6;
	default:	goto yy3;
	}
yy5:
	yych = *(YYMARKER = ++YYCURSOR);
	switch (yych) {
	case 'E':
	case 'e':	goto yy8;
	case 'T':
	case 't':	goto yy9;
	default:	goto yy3;
	}
yy6:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy10;
	default:	goto yy7;
	}
yy7:
	YYCURSOR = YYMARKER;
	goto yy3;
yy8:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy11;
	default:	goto yy7;
	}
yy9:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'A':
	case 'a':	goto yy12;
	default:	goto yy7;
	}
yy10:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy13;
	default:	goto yy7;
	}
yy11:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy14;
	default:	goto yy7;
	}
yy12:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy15;
	default:	goto yy7;
	}
yy13:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy16;
	case 'L':
	case 'l':	goto yy17;
	case 'M':
	case 'm':	goto yy18;
	case 'O':
	case 'o':	goto yy19;
	case 'P':
	case 'p':	goto yy20;
	default:	goto yy7;
	}
yy14:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy21;
	case 'F':
	case 'f':	goto yy22;
	case 'L':
	case 'l':	goto yy23;
	case 'M':
	case 'm':	goto yy24;
	case 'O':
	case 'o':	goto yy25;
	case 'P':
	case 'p':	goto yy26;
	default:	goto yy7;
	}
yy15:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'S':
	case 's':	goto yy27;
	default:	goto yy7;
	}
yy16:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy29;
	default:	goto yy7;
	}
yy17:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy30;
	default:	goto yy7;
	}
yy18:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'G':
	case 'g':	goto yy31;
	default:	goto yy7;
	}
yy19:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy32;
	default:	goto yy7;
	}
yy20:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy33;
	default:	goto yy7;
	}
yy21:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy34;
	default:	goto yy7;
	}
yy22:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy35;
	default:	goto yy7;
	}
yy23:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy36;
	default:	goto yy7;
	}
yy24:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'G':
	case 'g':	goto yy37;
	default:	goto yy7;
	}
yy25:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy38;
	default:	goto yy7;
	}
yy26:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy39;
	default:	goto yy7;
	}
yy27:
	++YYCURSOR;
	cond = yycdone;
	{ result = STATS; continue; }
yy29:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy40;
	default:	goto yy7;
	}
yy30:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'S':
	case 's':	goto yy41;
	default:	goto yy7;
	}
yy31:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'M':
	case 'm':	goto yy42;
	default:	goto yy7;
	}
yy32:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy43;
	default:	goto yy7;
	}
yy33:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy44;
	default:	goto yy7;
	}
yy34:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy45;
	default:	goto yy7;
	}
yy35:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'L':
	case 'l':	goto yy46;
	default:	goto yy7;
	}
yy36:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'S':
	case 's':	goto yy47;
	default:	goto yy7;
	}
yy37:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'M':
	case 'm':	goto yy48;
	default:	goto yy7;
	}
yy38:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy49;
	default:	goto yy7;
	}
yy39:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy50;
	default:	goto yy7;
	}
yy40:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy51;
	default:	goto yy7;
	}
yy41:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy52;
	default:	goto yy7;
	}
yy42:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy53;
	default:	goto yy7;
	}
yy43:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'G':
	case 'g':	goto yy54;
	default:	goto yy7;
	}
yy44:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'X':
	case 'x':	goto yy55;
	default:	goto yy7;
	}
yy45:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy56;
	default:	goto yy7;
	}
yy46:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy57;
	default:	goto yy7;
	}
yy47:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy58;
	default:	goto yy7;
	}
yy48:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy59;
	default:	goto yy7;
	}
yy49:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'G':
	case 'g':	goto yy60;
	default:	goto yy7;
	}
yy50:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'X':
	case 'x':	goto yy61;
	default:	goto yy7;
	}
yy51:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy62;
	default:	goto yy7;
	}
yy52:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy63;
	default:	goto yy7;
	}
yy53:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy64;
	default:	goto yy7;
	}
yy54:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy65;
	default:	goto yy7;
	}
yy55:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'Y':
	case 'y':	goto yy66;
	default:	goto yy7;
	}
yy56:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy67;
	default:	goto yy7;
	}
yy57:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy68;
	default:	goto yy7;
	}
yy58:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy69;
	default:	goto yy7;
	}
yy59:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy70;
	default:	goto yy7;
	}
yy60:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy71;
	default:	goto yy7;
	}
yy61:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'Y':
	case 'y':	goto yy72;
	default:	goto yy7;
	}
yy62:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy73;
	default:	goto yy7;
	}
yy63:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'N':
	case 'n':	goto yy74;
	default:	goto yy7;
	}
yy64:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'A':
	case 'a':	goto yy75;
	case 'P':
	case 'p':	goto yy76;
	default:	goto yy7;
	}
yy65:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'N':
	case 'n':	goto yy77;
	default:	goto yy7;
	}
yy66:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy78;
	default:	goto yy7;
	}
yy67:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy79;
	default:	goto yy7;
	}
yy68:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy80;
	default:	goto yy7;
	}
yy69:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'N':
	case 'n':	goto yy81;
	default:	goto yy7;
	}
yy70:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'A':
	case 'a':	goto yy82;
	case 'P':
	case 'p':	goto yy83;
	default:	goto yy7;
	}
yy71:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'N':
	case 'n':	goto yy84;
	default:	goto yy7;
	}
yy72:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy85;
	default:	goto yy7;
	}
yy73:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'F':
	case 'f':	goto yy86;
	default:	goto yy7;
	}
yy74:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy87;
	default:	goto yy7;
	}
yy75:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy88;
	default:	goto yy7;
	}
yy76:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy89;
	default:	goto yy7;
	}
yy77:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy90;
	default:	goto yy7;
	}
yy78:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'A':
	case 'a':	goto yy91;
	default:	goto yy7;
	}
yy79:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'F':
	case 'f':	goto yy92;
	default:	goto yy7;
	}
yy80:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy93;
	default:	goto yy7;
	}
yy81:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy95;
	default:	goto yy7;
	}
yy82:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy96;
	default:	goto yy7;
	}
yy83:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy97;
	default:	goto yy7;
	}
yy84:
	yych = *++YYCURSOR;
	switch (yych) {
	case '_':	goto yy98;
	default:	goto yy7;
	}
yy85:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'A':
	case 'a':	goto yy99;
	default:	goto yy7;
	}
yy86:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy100;
	default:	goto yy7;
	}
yy87:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'P':
	case 'p':	goto yy101;
	default:	goto yy7;
	}
yy88:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy102;
	default:	goto yy7;
	}
yy89:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy103;
	default:	goto yy7;
	}
yy90:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'P':
	case 'p':	goto yy104;
	default:	goto yy7;
	}
yy91:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy105;
	default:	goto yy7;
	}
yy92:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'I':
	case 'i':	goto yy106;
	default:	goto yy7;
	}
yy93:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy94;
		goto yy107;
	}
	goto yy108;
yy94:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_FILTER; *data = arg; *dataLen = end - arg; continue; }
yy95:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'P':
	case 'p':	goto yy109;
	default:	goto yy7;
	}
yy96:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy110;
	default:	goto yy7;
	}
yy97:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy111;
	default:	goto yy7;
	}
yy98:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'P':
	case 'p':	goto yy112;
	default:	goto yy7;
	}
yy99:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy113;
	default:	goto yy7;
	}
yy100:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'L':
	case 'l':	goto yy114;
	default:	goto yy7;
	}
yy101:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy115;
	default:	goto yy7;
	}
yy102:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy116;
	default:	goto yy7;
	}
yy103:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy118;
	default:	goto yy7;
	}
yy104:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy119;
	default:	goto yy7;
	}
yy105:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy120;
	default:	goto yy7;
	}
yy106:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'L':
	case 'l':	goto yy121;
	default:	goto yy7;
	}
yy107:
	yych = *++YYCURSOR;
yy108:
	switch (yych) {
	case '\n':	goto yy94;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy94;
		goto yy107;
	}
yy109:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy122;
	default:	goto yy7;
	}
yy110:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy123;
	default:	goto yy7;
	}
yy111:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy124;
	default:	goto yy7;
	}
yy112:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'O':
	case 'o':	goto yy125;
	default:	goto yy7;
	}
yy113:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'D':
	case 'd':	goto yy126;
	default:	goto yy7;
	}
yy114:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy127;
	default:	goto yy7;
	}
yy115:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy129;
	default:	goto yy7;
	}
yy116:
	++YYCURSOR;
	cond = yycdone;
	{ result = GET_MGMT_ADDR; continue; }
yy118:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy130;
	default:	goto yy7;
	}
yy119:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy132;
	default:	goto yy7;
	}
yy120:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy133;
	default:	goto yy7;
	}
yy121:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'E':
	case 'e':	goto yy135;
	default:	goto yy7;
	}
yy122:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy136;
	default:	goto yy7;
	}
yy123:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy137;
	default:	goto yy7;
	}
yy124:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy139;
	default:	goto yy7;
	}
yy125:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy141;
	default:	goto yy7;
	}
yy126:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'R':
	case 'r':	goto yy142;
	default:	goto yy7;
	}
yy127:
	++YYCURSOR;
	cond = yycdone;
	{ result = GET_ERROR_FILE; continue; }
yy129:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy143;
	default:	goto yy7;
	}
yy130:
	++YYCURSOR;
	cond = yycdone;
	{ result = GET_MGMT_PORT; continue; }
yy132:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy145;
	default:	goto yy7;
	}
yy133:
	++YYCURSOR;
	cond = yycdone;
	{ result = GET_PROXY_ADDR; continue; }
yy135:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy147;
	default:	goto yy7;
	}
yy136:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy149;
	default:	goto yy7;
	}
yy137:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy138;
		goto yy150;
	}
	goto yy151;
yy138:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_MGMT_ADDR; *data = arg; *dataLen = end - arg; continue; }
yy139:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy140;
		goto yy152;
	}
	goto yy153;
yy140:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_MGMT_PORT; *data = arg; *dataLen = end - arg; continue; }
yy141:
	yych = *++YYCURSOR;
	switch (yych) {
	case 'T':
	case 't':	goto yy154;
	default:	goto yy7;
	}
yy142:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy155;
	default:	goto yy7;
	}
yy143:
	++YYCURSOR;
	cond = yycdone;
	{ result = GET_LISTEN_PORT; continue; }
yy145:
	++YYCURSOR;
	cond = yycdone;
	{ result = GET_ORIGIN_PORT; continue; }
yy147:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy148;
		goto yy157;
	}
	goto yy158;
yy148:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_ERROR_FILE; *data = arg; *dataLen = end - arg; continue; }
yy149:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy159;
	default:	goto yy7;
	}
yy150:
	yych = *++YYCURSOR;
yy151:
	switch (yych) {
	case '\n':	goto yy138;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy138;
		goto yy150;
	}
yy152:
	yych = *++YYCURSOR;
yy153:
	switch (yych) {
	case '\n':	goto yy140;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy140;
		goto yy152;
	}
yy154:
	yych = *++YYCURSOR;
	switch (yych) {
	case ' ':	goto yy161;
	default:	goto yy7;
	}
yy155:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy156;
		goto yy163;
	}
	goto yy164;
yy156:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_PROXY_ADDR; *data = arg; *dataLen = end - arg; continue; }
yy157:
	yych = *++YYCURSOR;
yy158:
	switch (yych) {
	case '\n':	goto yy148;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy148;
		goto yy157;
	}
yy159:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy160;
		goto yy165;
	}
	goto yy166;
yy160:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_LISTEN_PORT; *data = arg; *dataLen = end - arg; continue; }
yy161:
	++YYCURSOR;
	yyt1 = YYCURSOR;
	yych = *YYCURSOR;
	if (yych <= 0x00) {
		if (YYLIMIT <= YYCURSOR) goto yy162;
		goto yy167;
	}
	goto yy168;
yy162:
	arg = yyt1;
	end = YYCURSOR;
	cond = yycdone;
	{ result = SET_ORIGIN_PORT; *data = arg; *dataLen = end - arg; continue; }
yy163:
	yych = *++YYCURSOR;
yy164:
	switch (yych) {
	case '\n':	goto yy156;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy156;
		goto yy163;
	}
yy165:
	yych = *++YYCURSOR;
yy166:
	switch (yych) {
	case '\n':	goto yy160;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy160;
		goto yy165;
	}
yy167:
	yych = *++YYCURSOR;
yy168:
	switch (yych) {
	case '\n':	goto yy162;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy162;
		goto yy167;
	}
yy169:
	{ return result; }
/* *********************************** */
yyc_done:
	yych = *YYCURSOR;
	switch (yych) {
	case '\n':	goto yy174;
	default:
		if (YYLIMIT <= YYCURSOR) goto yy176;
		goto yy172;
	}
yy172:
	++YYCURSOR;
	{ return ERROR; }
yy174:
	++YYCURSOR;
	{ return result; }
yy176:
	{ return result; }
}

		return ERROR;
	}
}
