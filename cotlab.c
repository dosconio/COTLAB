// GBK
#include <ustring.h>
#include <alice.h>
#include <aldbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <setjmp.h>
#include <conio.h>
#include "token.h"// need ustring

char buffer[0x1000];
char buffer2[0x1000];

//

typedef char arnbuf[0x1000];
arnbuf arna_tempor, arna_tmpslv, arna_tmpext;
size_t malc_limit = 0x1000, malc_occupy;

size_t malc_count;
size_t show_precise = 64;
//

extern char* SGAErroMsg;
extern jmp_buf errjb;
FILE* fp;
int fgetnext()
{
	if (!fp) return EOF;
	return fgetc(fp);
}
void fseekback(ptrdiff_t l)
{
	if (!fp) return;
	fseek(fp, l, SEEK_CUR);
}
int main()
{
	fopen_s(&fp, "d01.txt", "r");
	Tode* ori = StrTokenAll(fgetnext, fseekback, arna_tempor);
	if (!fp) return 1;
	fclose(fp);
	StrTokenStruct(ori);
}
int aaa[1 / 2 + 1];
/*
// For other application calling.
char* CotLine(const char* buffer, void(*ErrProc)(char*))
{
	Dnode* lineartok;
	token* tokfirst;
	const char* ptr = buffer;
	ptr = buffer;
	if (!(lineartok = StrTokenAllBuf((const char**)&ptr, buffer2)) || !(tokfirst = TokScpSyntax(TokScpLoad(lineartok))))
	{
		if (ErrProc) ErrProc("Bad system.");//TODO. There are not enough code to solve the memory block, in this version.
		return StrHeap("Bad system.");
	}
	if (ErrProc == erro) TokScpPrint(tokfirst);
	char* p = CalcNumOne(tokfirst);
	if (!p)
	{
		if (ErrProc) ErrProc("Bad expression.");
		return StrHeap("Bad expression.");
	}
	TokScpClr(tokfirst);
	return p;
}

int main(int argc, char** argv)
{
	int mode;
	Dnode* lineartok;
	token* tokfirst;
	init_total_errmech();
	if (argc > 1)// Load script argv[1]
	{
		mode = 'f';
		lineartok = StrTokenAllFile(argv[1], buffer2);
		if (!lineartok)
			puts("Cannot find!");
		if (!(tokfirst = TokScpSyntax(TokScpLoad(lineartok))))
			erro("Load fail!");
		TokScpPrint(tokfirst);
		num* res = CalcNum(tokfirst);
		if (!res) erro("Bad input");
		TokScpClr(tokfirst);
	}
	else
	{
		printf("COTLAB 1.1.1 [%d] [BASIC 2023] Lango : "__DATE__"\n", argc);// GCC only
		mode = 'c';
		while (1)
		{
			printf("%s", ">>> ");
			rewind(stdin);
			gets(buffer);
			if (!StrCompare(buffer, "exit")) break;
			char* res = CotLine(buffer, erro);
			printf("=== %s\n", res);
			memfree(res);
		}
	}
endo:;
	erro(0);
}*/
