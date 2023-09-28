// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

#include <stdio.h>
#include <cdear.h>
#include <inttypes.h>
#include "cotlab.h"
#include "parser.h"
#include "executor.h"

static size_t cabort_col, cabort_row;
static char* cabort_txt;

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
		if (crt->data && crt->type == dt_float)
			printf("%s <%s%s> %lf\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], CoeToDouble((void*)crt->data));
		else if(crt->data && crt->type == dt_str)
			printf("%s <%s%s> \"%s\"\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], (char*)crt->data);
		else
			printf("%s <%s%s> %s\n", crt->addr, crt->property ? "const " : "",
				tokentype_iden[crt->type], crt->data ? (char*)crt->data : "#");
		crt = crt->right;
	}
}

void NnodePrint(const Nesnode* nnod, unsigned nest)
{
	const Nesnode* crt = nnod;
	while (crt)
	{
		for (unsigned i = 0; i < nest; i++) putchar('\t');
		if (crt->class == dt_float)
			printf("Nnode [%s] %lf\n", tokentype_iden[crt->class], CoeToDouble((void*)crt->addr));
		else
			printf("Nnode [%s] %s\n", tokentype_iden[crt->class], crt->addr);
		if (crt->subf) NnodePrint(crt->subf, nest + 1);
		crt = crt->right;
	}
}

void CotPrint(tnode* inp)
{
	CoeInit();
	for (tnode* crt = inp->right; crt; crt = crt->next)
	{
		if (crt->type == tok_number)
		{
			char* p;
			p = CoeToLocale((void*)crt->addr, 1);
			printf("Line %" PRIiPTR ": %s\n", crt->row + 1, p);//{TODO} use tnode as dnode
			memf(p);
		}
		else if (crt->type == tok_string)
		{
			printf("Line %" PRIiPTR ": \"%s\"\n", crt->row + 1, crt->addr);
		}
		else
		{
			if (crt->addr)
				printf("Line %" PRIiPTR ": %s\n", crt->row + 1, crt->addr);//{TODO} use tnode as dnode
		}
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
		InodeUpdate(inods[1], "last", (void*)CoeCpy((void*)res->addr), dt_float, 0x80, InodeReleaseTofreeElementCotlab);
	else
		InodeUpdate(inods[1], "last", StrHeap(res->addr), res->type, 0x80, InodeReleaseTofreeElementCotlab);
}

// Return 1 for delete current, 0 for none, 2 for clear all.
int CotApplyObject(nnode* inp, nnode* parent)
{
	//{TEMP} just snesitive inod-chain
	nnode* crt = inp;
	if (crt->class != tok_iden || !crt->addr) return 0;
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
			else srs(crt->addr, StrHeap(des->data));
			crt->class = des->type;
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
		next = crt->right;
		if (!CotExecuate(crt->subf, crt))
			if (parent)	return 0;
			else goto errp;
		if (crt->class == tok_iden)// No else case with the below
			if(state = CotApplyObject(crt, parent))
			{
				if (state == 2)
					if (parent)	return 0;
					else goto errp;
				next = crt->right;
				NnodeRelease(crt, parent, 0);
				continue;
			}
		if (crt->class == tok_any)continue;
		if (!crt->subf && !crt->bind)
		{
			dnode* crttmp = NnodeToDnode(MemHeap(crt, sizeof *crt));
			if (!parent) CotUpdateLast(crttmp);
			memf(crttmp);
			continue;
		}
		// f(h(1,2)); pi(); (1);
		Dnode* f_in = NnodeToDnode(crt->subf);// one of alias of crt->subf
		Dnode* res = 0;
		if (crt->bind)
		{
			res = ((fstruc_t)crt->bind)(f_in);
			DnodesReleaseTofreeCotlab(f_in);
		}
		else res = f_in;
		if (!parent) CotUpdateLast(res);
		if (res)
		{
			//{TEMP} just for 1 return.
			crt->class = res->type;
			crt->subf = 0;
			if (crt->addr) memf(crt->addr);
			crt->addr = res->addr;
			DnodeRelease(res, 0);//{TODO} now just for single return function
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
