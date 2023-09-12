// ASCII GNU-GPL v3.0
// COTLAB  Copyright (C) 2023  Dosconio
// This is free software, and you are welcome to redistribute it
// under certain conditions; type `$ cot license' for details.

#define _LIB_STRING_HEAP
#include <ustring.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h>
#include <conio.h>
#include "parser.h"// need ustring
#include "executor.h"
ulibsym(0x1000)

char buffer[0x1000];
char buffer2[0x1000];
//
typedef char arnbuf[0x1000];
arnbuf arna_tempor, arna_tmpslv, arna_tmpext;
//
extern char* SGAErroMsg;
extern jmp_buf errjb;
FILE* fp;
char* ptr;
int fgetnext() { return fgetc(fp); }
void fseekback(ptrdiff_t l) { fseek(fp, l, SEEK_CUR); }
int sgetnext() { return (*ptr) ? *ptr++ : EOF; }
void sseekback(ptrdiff_t l) { ptr += l; }

int main(int argc, char** argv)
{
	int option = 0;//1[shell] 2[command] 3[script]
	Tode* ori;
	
	if (argc <= 1 || !StrCompare(argv[1], "-s")) option = 1;
	else if (!StrCompare(argv[1], "-f")) option = 3;
	else if (!StrCompare(argv[1], "-c")) option = 2;
	else puts("Unknown command.");
	switch (option)
	{
	case 1:
		printf("COTLAB <GPL-3> [DATURA] [SGA-3] Copyright (C) 2023 @dosconio ayano~ " __DATE__ "\n");
		puts("Shell is going to be realized...");
		break;
	case 2:
		if (argc < 3)
		{
			printf("Command at least one should be input.");
			break;
		}
		ptr = argv[2];
		ori = StrTokenAll(sgetnext, sseekback, arna_tempor);
		StrTokenParse(ori);
		CotExecuate(ori, NULL, NULL);
		// {TODO} clear
		break;
	case 3:// script
		if (argc < 3)
		{
			printf("File at least one should be input.");
			break;
		}
		fopen_s(&fp, argv[2], "r");
		if (!fp)
		{
			printf("File %s is misty.", argv[2]);
			break;
		}
		ori = StrTokenAll(fgetnext, fseekback, arna_tempor);
		fclose(fp);
		StrTokenParse(ori);
		CotExecuate(ori, NULL, NULL);
		// {TODO} clear
		break;
	default:
		break;
	}
	
	printf("Oyasuminasaiii~ with malc_count = %llx\n", malc_count);
}

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
