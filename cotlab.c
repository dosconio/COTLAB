// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// This is free software, and you are welcome to redistribute it
// under certain conditions; type `$ cot license' for details.

#include "ulibex.h"
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
	nnode* res;

	init_total_errmech(1);
	
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
		res = StrTokenParse(ori);
		CotExecuate(res);
		ori = NnodeToDnode(res);
		CotPrint(ori);
		CotRelease(ori);
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
		res = StrTokenParse(ori);
		CotExecuate(res);
		ori = NnodeToDnode(res);
		CotPrint(ori);
		CotRelease(ori);
		break;
	default:
		break;
	}
	
	if(malc_count) printf("System leak: Oyasuminasaiii~ with malc_count = %llx\n", malc_count);
}
