// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// This is free software, and you are welcome to redistribute it
// under certain conditions; type `$ cot help' for details.

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <coear.h>
#include <consio.h>
#include "cotlab.h"
#include "parser.h"
#include "executor.h"
#include <error.h>

#define COT_SHELL_MODE_FAST 1

char* COT_CRTFILE;// Current file name

//
typedef char arnbuf[0x1000];
arnbuf arna_tempor,// parse pool
	arna_tmpslv,// command pool
	arna_tmpext;// work-ing folder pool

inode** inods = 0;
int state;

int main(int argc, char** argv)
{

	
	// ---- ---- pre-set constants ---- ---- 
	sensi_objs = InodeUpdate(0, "last", 0, 0, tok_EOF, InodeReleaseTofreeElementCotlab);// LOCK
	InodeUpdate(sensi_objs, "pi", (void*)CoePi(), tok_number, 1, InodeReleaseTofreeElementCotlab);
	InodeUpdate(sensi_objs, "e", (void*)CoeE(), tok_number, 1, InodeReleaseTofreeElementCotlab);

	// ---- ---- main ---- ---- 
apply_opt:
	switch (option)
	{
	case 1:
		COT_EXE_AUTOLF = 1;
		while (1)
		{
			if (mode == COT_SHELL_MODE_FAST)
			{
				printf("\n>>> ");
			}
			else if (!mode)
			{
				(void)ConGetCurrentDirectory(arna_tmpext, sizeof arna_tmpext);
				if (arna_tmpext[1] == ':')
				{
					size_t path_len = 0;
					arna_tmpext[1] = '>';
					while(arna_tmpext[path_len])
					{
						if (arna_tmpext[path_len] == '\\')
							arna_tmpext[path_len] = '/';
						path_len++;
					}
					MemRelative(arna_tmpext + 3, path_len - 2, -1);
					printf("\n>%s> ", arna_tmpext);
				}
				else printf("\n<%s> ", arna_tmpext);
			}
			else break;
			ConScanLine(arna_tmpslv, sizeof arna_tmpslv);
			

			if (!StrCompare(arna_tmpslv, "mode") && mode != COT_SHELL_MODE_FAST)
			{
				puts("FAST - MODE!");
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
			else if (!StrCompareN(arna_tmpslv, "cd ", 3))
			{
				if (ConSetCurrentDirectory(arna_tmpslv + 3))// not system(arna_tmpslv);
					fprintf(stderr, "Invaild Working Path.");// cabort is for cottask
				continue;
			}
			else if (!StrCompare(arna_tmpslv, "dir"))
			{
				system("dir");
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
	case 4:
		cottask(0, ptr);
		option = 1;
		goto apply_opt;
		break;// formal
	default:
		break;
	}
	InodesRelease(sensi_objs, InodeReleaseTofreeCotlab);
	//{TODO} InodesRelease(isens_objs, InodeReleaseTofreeCotlab);
	
	
}
