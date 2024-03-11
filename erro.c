// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// Error and destruction handling
#pragma warning(disable:4005)// redefine of macro
#pragma warning(disable:6011)
#define _LIB_STRING_HEAP

#include <error.h>
#include <inttypes.h>
#include <stdio.h>
#include <ustring.h>
#include <consio.h>

#include <coear.h>
#include <numar.h>
#include "parser.h"

extern char* COT_CRTFILE;// Current file name

// Cotlab Abort
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

// Destructure object according to type
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
	CotResourceRemove(nod->addr, nod->type);
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

void DnodesReleaseCotlab(dnode* inp)
{
	CotResourceRemove(inp->addr, inp->type);
	memf(inp);
}

void TnodesReleaseTofreeCotlab(void* inp)
{
	tnode* first = inp;
	CotResourceRemove(first->addr, first->type);
	memfree(first);
}

