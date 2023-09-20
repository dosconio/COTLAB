// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// This is free software, and you are welcome to redistribute it
// under certain conditions; type `$ cot license' for details.

#include "ulibex.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h>
#include <conio.h>
#include "cotlab.h"
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
size_t tasks = 0;
int fgetnext() { return fgetc(fp); }
void fseekback(ptrdiff_t l) { fseek(fp, l, SEEK_CUR); }
int sgetnext() { return (*ptr) ? *ptr++ : EOF; }
void sseekback(ptrdiff_t l) { ptr += l; }
//


// src[0:file, 1:textbuf]
int cottask(int src, void* point)
{
	tasks++;
	jmp_buf errjb_stack;
	MemCopyN(&errjb_stack, &errjb, sizeof errjb);
	init_total_errmech(1);
	inode* pre_macros = 0;// > nsens_objs
	inode* nsens_objs = 0;// > sensi_objs
	inode* sensi_objs = 0;
	void* stack_last_pointer_file = fp;
	void* stack_last_pointer_str = ptr;
	tnode* ori;
	nnode* res;
	
	// ---- ---- pre-set constants ---- ---- 
	///sensi_objs = zalcof(inode);

	// ---- ---- context ---- ----
	if (src) ptr = point;
	else
	{
		fp = fopen(point, "r");
		if (!fp)
		{
			printf("File %s is misty.", point);
			if (!fp) goto endo;
		}
	}
	ori = StrTokenAll(!src ? fgetnext : sgetnext,
		!src ? fseekback : sseekback, arna_tempor);
	res = StrTokenParse(ori);
	CotExecuate(res);
	ori = NnodeToTnode(res);
	CotPrint(ori);
	TnodesReleases(ori, TnodesReleaseTofreeCotlab);

endo:
	if (!src && !fp) fclose(fp);
	///DnodesRelease(sensi_objs,)
	ptr = stack_last_pointer_str;
	fp = stack_last_pointer_file;
	MemCopyN(&errjb, &errjb_stack, sizeof errjb);
	tasks--;
}

int main(int argc, char** argv)
{
	int option = 0;//1[shell] 2[command] 3[script]
	init_total_errmech(1);
	ptr = argc < 3 ? 0 : argv[2];
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
		printf(CotTitle);
		while (1)
		{
			printf("\n>>> ");
			gets_s(arna_tmpslv, 0x1000);
			if (!StrCompare(arna_tmpslv, "exit")) break;
			if (!StrCompare(arna_tmpslv, "cls"))
			{
#ifdef _WinNT
				system("cls");
#elif defined(_Linux)
				system("clear");// {TODO} test
#endif
				continue;;
			}
			cottask(1, arna_tmpslv);
		}
		break;
	case 2:
		cottask(1, ptr);
		break;
	case 3:// script
		cottask(0, ptr);
		break;
	default:
		break;
	}
	if(malc_count) printf("System leak: %llu times!\n", malc_count);
}
