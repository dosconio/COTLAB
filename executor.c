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

void InodePrint(inode*** inp)
{
	// inp zo inode*[3]*; 
	//{}inode* pre_macros = inp[0][0];
	inode* sensi_objs = inp[0][1];
	//{}inode* isens_objs = inp[0][2];
	puts("");
	//{}puts("Here are sensitive objects:");
	inode* crt = sensi_objs;
	while (crt)
	{
		if (!crt->data)
		{
			printf("%s <%s%s> %s\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], "#null");
		}
		else if (crt->type == dt_float)
			printf("%s <%s%s> %lf\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], CoeToDouble((void*)crt->data));
		else if (crt->type == dt_num)
		{
			printf("%s <%s%s> [%lf %lfj]\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], CoeToDouble(&((numa*)crt->data)->Real), CoeToDouble(&((numa*)crt->data)->Imag));
		}
		else if(crt->type == dt_str)
			printf("%s <%s%s> \"%s\"\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], (char*)crt->data);
		else
			printf("%s <%s%s> %s\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], (char*)crt->data);
		crt = crt->next;
	}
}

void NnodePrint(const nnode* nnod, unsigned nest)
{
	const nnode* crt = nnod;
	while (crt)
	{
		for (unsigned i = 0; i < nest; i++) putchar('\t');
		if (crt->type == dt_float)
			printf("Nnode [%s] %lf\n", tokentype_iden[crt->type], CoeToDouble((void*)crt->addr));
		else if (crt->type == dt_num)
		{
			printf("Nnode [%s] [%lf %lfj]\n", tokentype_iden[crt->type], CoeToDouble(&((numa*)crt->addr)->Real), CoeToDouble(&((numa*)crt->addr)->Imag));
		}
		else
			printf("Nnode [%s] %s\n", tokentype_iden[crt->type], crt->addr);
		if (crt->subf) NnodePrint(crt->subf, nest + 1);
		crt = crt->next;
	}
}

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

int CotExecuate(nnode* inp, nnode* parent)// use parent to replace return nnode*
{
	// Nest ---> Linear of (func, imm-value)
	// (1) ---> 1;  pi() ---> 3.1416; ...
	//{TODO} Multi-in and multi-out
	int state = 0;
	CoeInit();
	nnode* next = 0;
	for (nnode* crt = inp; crt; crt = next)
	{
		next = crt->next;
		if (!CotExecuate(crt->subf, crt))
			if (parent)	return 0;
			else goto errp;
		if (crt->type == tok_identy)// No else case with the below
			if(state = CotApplyObject(crt, parent))
			{
				if (state == 2)
					if (parent)	return 0;
					else goto errp;
				next = crt->next;
				NnodeRelease(crt, parent, 0);
				continue;
			}
		if (crt->type == tok_any)continue;
		if (!crt->subf && !crt->bind)
		{
			dnode* crttmp = NnodeToDnode(MemHeap(crt, sizeof *crt));
			if (!parent) CotUpdateLast(crttmp);
			memf(crttmp);
			continue;
		}
		// f(h(1,2)); pi(); (1);
		dnode* f_in = NnodeToDnode(crt->subf);// one of alias of crt->subf
		dnode* res = 0;
		if (crt->bind)
		{
			res = ((fstruc_t)crt->bind)(f_in);
			DnodesReleaseCotlab(f_in);
		}
		else res = f_in;
		if (!parent) CotUpdateLast(res);
		if (res)
		{
			//{TEMP} just for 1 return.
			crt->type = res->type;
			crt->subf = 0;
			if (crt->addr) memf(crt->addr);
			crt->addr = res->addr;
			_dnode_freefunc = nil, DnodeRemove(res);
		}
		else
		{
			// Remove the node
			// Linear, there should always be a element on the left
			// Catious: for script file, the value are batch-executed but show-time
			nnode* tmp = crt->left;
			crt->subf = 0;
			NnodeRelease(crt, 0, NnodeReleaseTofreeDefault);
			crt = tmp;
		}
	}
	return 1;
errp:
	NnodesRelease(inp, 0, NnodeReleaseTofreeCotlab);
	cabort("Undefined identifier", cabort_row, cabort_col, cabort_txt);// current only reason
	cabort_txt = 0;
	return 0;
}
