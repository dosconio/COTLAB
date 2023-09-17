// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// Complement for Hstring/Nest-Node.
#include "ulibex.h"
#include <stdio.h>
#include <cdear.h>

void NnodeFree(void* n)
{
	nnode* nod = n;
	if (nod->class == tok_number)
		CoeDel((void*)nod->addr);
	else memf(nod->addr);
	memf(n);
}

void DnodesReleaseCotlab(dnode* inp)
{
	dnode* first = inp;
	if (inp->next) DnodesReleaseCotlab(inp->next);
	if (first->type == tok_number)
		CoeDel((void*)first->addr);
	else if (first->addr) memfree(first->addr);
	memfree(first);
}

static void TnodesReleaseCotlab(void* inp)
{
	tnode* first = inp;
	if (first->type == tok_number)
		CoeDel((void*)first->addr);
	else if(first->addr) memfree(first->addr);
	memfree(first);
}

void CotRelease(tnode* inp)//{TODO} tnode
{
	//NnodesRelease(inp, 0, NnodesReleaseCotlab);// can run but not explicit
	TnodesReleases(inp, TnodesReleaseCotlab);
}

void NnodePrint(const Nesnode* nnod, unsigned nest)
{
	const Nesnode* crt = nnod;
	while (crt)
	{
		for (unsigned i = 0; i < nest; i++) putchar('\t');
		if (crt->class == tok_number)
			printf("Nnode [%s] %lf\n", crt->class >= 16 ? datatype_iden[crt->class - 16] : "NNODE", CoeToDouble((void*)crt->addr));
		else
			printf("Nnode [%s] %s\n", crt->class >= 16 ? datatype_iden[crt->class - 16] : "NNODE", crt->addr);
		if (crt->subf) NnodePrint(crt->subf, nest + 1);
		crt = crt->right;
	}
}

int NnodeSymbolsDivide(nnode* inp, size_t width, size_t idx, nnode* parent)
{
	// RFV14
	size_t slen = StrLength(inp->addr);
	// i+++j, +++, "+" idx(2)width(1)slen(3)
	if (slen == 0 || idx + width > slen) erro("SYSTEM ERROR: yo NODEX->NnodeSymbolsDivide");
	// 0case .
	if (slen == width) return 0;// assert (idx zo 0)
	// 1case .@@
	// "a" "+++" "b", "a" "++"-'+'-"b"
	if (idx == 0)
	{
		nnode* newd = zalc(sizeof(nnode));
		newd->class = tok_sym;
		newd->row = inp->row;
		newd->col = inp->col + width;
		newd->addr = StrHeap(inp->addr + width);
		inp->addr[width] = 0;
		newd->right = inp->right;
		newd->left = inp;
		if (inp->right) inp->right->left = newd;
		inp->right = newd;
		return 1;
	}
	// 2case @@.
	// "a" "+++" "b", "a"-'++'-"+" "b"
	if (idx + width == slen)
	{
		nnode* newd = NnodeInsert(inp, 0, parent);
		newd->class = tok_sym;
		inp->col += slen - width;
		char* tmpaddr = StrHeap(inp->addr + idx);
		newd->addr = inp->addr;
		newd->addr[slen - width] = 0;
		inp->addr = tmpaddr;
		return 2;
	}
	// 3else @.@
	// "a" "+++*" "b", "a"-'++'-"+"-'*'-"b"
	nnode* newleft = NnodeInsert(inp, 0, parent);
	nnode* newright = NnodeInsert(inp, 1, parent);
	newleft->class = newright->class = tok_sym;
	newleft->col = inp->col;
	inp->col += idx;
	newright->col = inp->col + width;
	char* tmpaddr_mid = StrHeapN(inp->addr + idx, width);
	newright->addr = StrHeap(inp->addr + idx + width);// idx zo lenof left
	newleft->addr = inp->addr;
	newleft->addr[idx] = 0;
	inp->addr = tmpaddr_mid;

	return 3;
}

