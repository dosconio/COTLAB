// ASCII
#pragma warning(disable:4005)// redefine of macro
#pragma warning(disable:6011)
#define _LIB_STRING_HEAP
#include <setjmp.h>
#include <inttypes.h>
#include <stdio.h>
#include <ustring.h>
#include <cdear.h>
#include <numar.h>
#include <consio.h>
#include "parser.h"

Dnode* SGAWarnChain;//{TODO} warnings chain
size_t SGANumofWarn = 0;

char* SGAErroMsg = 0;
jmp_buf errjb = { 0 };

Dnode* ToFreeList;//{TODO}
char* COT_CRTFILE;

void warn(char* str)
{
	SGANumofWarn++;
	printf("Warn %s\n", str);
	///if (!SGAFirstWarn) SGAFirstWarn = str;
}
void erro(char* str)
{
	SGAErroMsg = str;
	printf("Erro: %s \n", str);//{TEMP OCCUPY}
	return;//{TEMP OCCUPY}
	if (errjb)
		longjmp(errjb, 1);
}

void cabort(const char* str, size_t row, size_t col, char* txt)
{
	row++;
	ConStyleAbnormal();
	if (!COT_CRTFILE)
		fprintf(stderr, "Error R%" PRIuPTR " C%" PRIuPTR , row, col);
	else fprintf(stderr, "Error F\"%s\" R%" PRIuPTR " C%" PRIuPTR, COT_CRTFILE, row, col);
	ConStyleNormal();
	fprintf(stderr, " %s", str);
	if (txt) 
	{
		fprintf(stderr, " (%s)", txt);
		memf(txt);
	}
	fprintf(stderr, "\n");
	if (errjb) longjmp(errjb, 1);
}

inline static void CotResourceRemove(void* obj, size_t typ)
{
	if (!obj) return;
	switch (typ)
	{
	case tok_number:
		CoeDel(obj);
		break;
	case dt_num:
		NumDel(obj);
		break;
	default:
		memf(obj);
		break;
	}
}

void NnodeReleaseTofreeCotlab(void* n)
{
	nnode* nod = n;
	CotResourceRemove(nod->addr, nod->class);
	memf(n);
}

void InodeReleaseTofreeElementCotlab(void* n)
{
	if (!n) return;
	inode* nod = n;
	CotResourceRemove(nod->data, nod->type);
	nod->data = 0;
}

void InodeReleaseTofreeCotlab(void* n)
{
	if (!n) return;
	inode* nod = n;
	memf(nod->addr);
	CotResourceRemove(nod->data, nod->type);
	memf(nod);
}

// !!! call by just COTLAB but passed for unisym, so this operate multi-items
void DnodesReleaseCotlab(dnode* inp)
{
	dnode* first = inp;
	if (!inp) return;
	if (inp->next) DnodesReleaseCotlab(inp->next);
	CotResourceRemove(first->addr, first->type);
	memfree(first);
}

void TnodesReleaseTofreeCotlab(void* inp)
{
	tnode* first = inp;
	CotResourceRemove(first->addr, first->type);
	memfree(first);
}

