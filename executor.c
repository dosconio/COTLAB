// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

#include <stdio.h>
#include <coear.h>
#include <numar.h>
#include <inttypes.h>
#include "cotlab.h"
#include "parser.h"
#include "executor.h"

static size_t cabort_col, cabort_row;
static char* cabort_txt;
int COT_EXE_AUTOLF;




void CotPrint(tnode* inp)// Linear
{
	int single = inp->next && !inp->next->next;
	CoeInit();
	for (tnode* crt = inp->next; crt; crt = crt->next)
	{
		if (!crt->addr) continue;
		if(!single) printf("Line %" PRIiPTR ": ", crt->row + 1);
		if (crt->type == dt_float)
		{
			char* p;
			p = CoeToLocale((void*)crt->addr, 1);
			printf("%s", p);
			memf(p);
		}
		else if (crt->type == dt_num)
		{
			char* p0, *p1; numa* n = (numa*)crt->addr;
			p0 = CoeToLocale(&n->Real, 1);
			p1 = CoeToLocale(&n->Imag, 1);
			printf("[%s %s]", p0, p1);
			memf(p0); memf(p1);
		}
		else if (crt->type == dt_str)
		{
			printf("\"%s\"", crt->addr);
		}
		else
		{
			printf("%s", crt->addr);
		}
		if (COT_EXE_AUTOLF || !single) puts("");
	}
}

static void CotUpdateLast(dnode* res)
{
	if (!res) return;
	if (!res->addr)
	{
		InodeUpdate(inods[1], "last", 0, 0, 0x80, InodeReleaseTofreeElementCotlab);
		return;
	}
	if (res->type == dt_float)
		InodeUpdate(inods[1], "last", (void*)CoeCpy((void*)res->addr), res->type, 0x80, InodeReleaseTofreeElementCotlab);
	else if(res->type == dt_num)
		InodeUpdate(inods[1], "last", (void*)NumCpy((void*)res->addr), res->type, 0x80, InodeReleaseTofreeElementCotlab);
	else
		InodeUpdate(inods[1], "last", StrHeap(res->addr), res->type, 0x80, InodeReleaseTofreeElementCotlab);
}

// Return 1 for delete current, 0 for none, 2 for clear all.
int CotApplyObject(nnode* inp, nnode* parent)
{
	//{TEMP} just snesitive inod-chain
	nnode* crt = inp;
	if (crt->type != tok_identy || !crt->addr) return 0;
	if (!(parent && !StrCompare(parent->addr, "ASSIGN") && parent->subf == crt))
	{
		inode* des = InodeLocate(inods[1], crt->addr, 0);
		if (des)
		{
			if (!des->data) return 1;// empty
			if (des->type == tok_number)
			{
				srs(crt->addr, CoeCpy(des->data));
			}
			else if (des->type == dt_num)
			{
				srs(crt->addr, NumCpy(des->data));
			}
			else srs(crt->addr, StrHeap(des->data));
			crt->type = des->type;
		}
		else
		{
			cabort_col = crt->col - (crt->addr ? StrLength(crt->addr) : 0);
			cabort_row = crt->row;
			cabort_txt = StrHeap(crt->addr);
			return 2;
		}
	}
	return 0;
}

