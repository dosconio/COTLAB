// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// This is free software, and you are welcome to redistribute it
// under certain conditions; type `$ cot license' for details.

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <setjmp.h>
#include <cdear.h>
#include <consio.h>
#include "cotlab.h"
#include "parser.h"// need ustring
#include "executor.h"
ulibsym(0x1000)

#define COT_SHELL_MODE_FAST 1

//
typedef char arnbuf[0x1000];
arnbuf arna_tempor,// parse pool
	arna_tmpslv,// command pool
	arna_tmpext;// work-ing folder pool
//
extern char* SGAErroMsg;
extern jmp_buf errjb;
FILE* fp;
char* ptr;
size_t tasks = 0;
inode** inods = 0;
int state;
int fgetnext(void) { return fgetc(fp); }
void fseekback(ptrdiff_t l) { fseek(fp, (long)l, SEEK_CUR); }
int sgetnext(void) { return (*ptr) ? *ptr++ : EOF; }
void sseekback(ptrdiff_t l) { ptr += l; }
//


// Return zero for success for init_total_errmech. src[0:file, 1:textbuf]
int cottask(int src, void* point)// {TODO} receive varlist inode[3]*
{
	extern char* COT_CRTFILE;
	COT_CRTFILE = src ? 0 : point;
	tasks++;
	jmp_buf errjb_stack;
	MemCopyN(&errjb_stack, &errjb, sizeof errjb);
	init_total_errmech(0);
	void* stack_last_pointer_file = fp;
	void* stack_last_pointer_str = ptr;
	tnode* ori;
	nnode* res;

	if (src) ptr = point;
	else if (!(fp = fopen(point, "r")))
	{
		printf("File %s is misty.", (char*)point);
		goto endo;
	}
	ori = StrTokenAll(!src ? fgetnext : sgetnext,
		!src ? fseekback : sseekback, arna_tempor);
	if (ori->right && !ori->right->right)// single iden ---> func
	{
		if (ori->right->type == tok_iden && !InodeLocate(inods[1], ori->right->addr, 0))
			StrTokenAppend(ori->right, "()", 2, tok_sym, 0, ori->right->col + 2);
	}// {FUTURE}[convert `iden entity0 ... entityN` to iden(...)] sin 1 ---> sin(1)
	res = StrTokenParse(ori);// {TODO} param:inodes [data:0] for del item
	state = CotExecuate(res, 0);
	(void)state;
	ori = NnodeToTnode(res);
	CotPrint(ori);
	TnodesReleases(ori, TnodesReleaseTofreeCotlab);
endo:
	if (!src && fp) fclose(fp);
	ptr = stack_last_pointer_str;
	fp = stack_last_pointer_file;
	MemCopyN(&errjb, &errjb_stack, sizeof errjb);
	tasks--;
	return 0;
}

int main(int argc, char** argv)
{
	int option = 0;//1[shell] 2[command] 3[script]
	int mode = 0;
	inode* pre_macros = 0;// > nsens_objs
	inode* sensi_objs = 0;// > isens_objs
	inode* isens_objs = 0;// inode::property as [...|MUTABLE]
	init_total_errmech(1);
	CoeInit();
	ConStyleNormal();
	// ---- ---- pre-set constants ---- ---- 
	sensi_objs = InodeUpdate(0, "last", 0, 0, tok_EOF, InodeReleaseTofreeElementCotlab);// LOCK
	InodeUpdate(sensi_objs, "pi", (void*)CoePi(), tok_number, 1, InodeReleaseTofreeElementCotlab);
	InodeUpdate(sensi_objs, "e", (void*)CoeE(), tok_number, 1, InodeReleaseTofreeElementCotlab);

	inods = (inode * []){ pre_macros, sensi_objs, isens_objs };
	// ---- ---- main ---- ---- 
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
			if (mode == COT_SHELL_MODE_FAST)
			{
				printf(">>> ");
			}
			else if (!mode)
			{
				(void)ConCurrentDirectory(arna_tmpext, sizeof arna_tmpext);
				printf("\n<%s> ", arna_tmpext);
			}
			else break;
			ConScanLine(arna_tmpslv, sizeof arna_tmpslv);
			if (!StrCompare(arna_tmpslv, "exit")) break;
			if (!StrCompare(arna_tmpslv, "help"))
			{
				puts("\nCOTLAB Generation 3, GNU General Public License 3 (cotlab.org)\n"
					"  * Weaver Dosconio (dosconyo@gmail.com / doscon.io)\n"
					"  * Models UNISYM");
				continue;
			}
			if (!StrCompare(arna_tmpslv, "fast"))
			{
				puts("FASTER-MODE!");
				mode = COT_SHELL_MODE_FAST;
				LIB_CDE_PRECISE_SHOW = 6;
				continue;
			}
			if (!StrCompare(arna_tmpslv, "cls"))
			{
#ifdef _WinNT
				system("cls");
#elif defined(_Linux)
				system("clear");// {TODO} test
#endif
				continue;
			}
			if (!StrCompare(arna_tmpslv, "list"))
			{
				InodePrint(&inods);
				continue;
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
	InodesRelease(sensi_objs, InodeReleaseTofreeCotlab);
	//{TODO} InodesRelease(isens_objs, InodeReleaseTofreeCotlab);
	if(malc_count) printf("System leak: %" PRIiPTR " times!\n", malc_count);
}
