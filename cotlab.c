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

#ifdef __GNUC__
#define gets_s(str,len) gets(str)
#endif

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
	ptr = argc < 3 ? 0 : argv[2];
	init_total_errmech(1);
	
	if (argc <= 1 || !StrCompare(argv[1], "-s")) option = 1;
	else if (argc >= 3 && !StrCompare(argv[1], "-f")) option = 3;
	else if (argc >= 3 && !StrCompare(argv[1], "-c")) option = 2;
	else if (argc > 1)
	{
		FILE* tmpf = fopen(argv[1], "r");
		ptr = argv[1];
		if (tmpf)
		{
			option = 3;
			fclose(tmpf);
		}
		else option = 2;
	}
	else option = 1;
	// else puts("Unknown command.");
	switch (option)
	{
	case 1:
		printf("COTLAB [Datura] Copyright @Dosconio~ Build " __DATE__ "\n");
		while (1)
		{
			printf("\n>>> ");
			gets_s(ptr = arna_tmpslv, 0x1000);
			if (!StrCompare(arna_tmpslv, "exit")) break;
			ori = StrTokenAll(sgetnext, sseekback, arna_tempor);
			res = StrTokenParse(ori);
			CotExecuate(res);
			ori = NnodeToTnode(res);
			CotPrint(ori);
			CotRelease(ori);
		}
		break;
	case 2:
		ori = StrTokenAll(sgetnext, sseekback, arna_tempor);
		res = StrTokenParse(ori);
		CotExecuate(res);
		ori = NnodeToTnode(res);
		CotPrint(ori);
		CotRelease(ori);
		break;
	case 3:// script
		fopen_s(&fp, ptr, "r");
		if (!fp)
		{
			printf("File %s is misty.", ptr);
			break;
		}
		ori = StrTokenAll(fgetnext, fseekback, arna_tempor);
		fclose(fp);
		res = StrTokenParse(ori);
		CotExecuate(res);
		ori = NnodeToTnode(res);
		CotPrint(ori);
		CotRelease(ori);
		break;
	default:
		break;
	}
	
	if(malc_count) printf("System leak: Oyasuminasaiii~ with malc_count = %llx\n", malc_count);
}
