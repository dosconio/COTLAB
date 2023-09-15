// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// Complement for Hstring/Nest-Node.
#include "ulibex.h"
#include <stdio.h>
#include <cdear.h>

static void DnodesRelease_Chg(Dnode* first)
{
	if (first->next) DnodesRelease_Chg(first->next);
	if (first->type == tok_number)
		CoeDel(first->addr);
	else memfree(first->addr);
	memfree(first);
}

void CotRelease(dnode* inp)//{TODO} tnode
{
	DnodesRelease_Chg(inp);
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

int NnodeSymbolsDivide(nnode* inp, size_t width, size_t idx)
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
		nnode* newd = zalc(sizeof(nnode));
		newd->class = tok_sym;
		newd->row = inp->row;
		newd->col = inp->col;
		inp->col += slen - width;
		char* tmpaddr = StrHeap(inp->addr + idx);
		newd->addr = inp->addr;
		newd->addr[slen - width] = 0;
		inp->addr = tmpaddr;
		newd->right = inp;
		newd->left = inp->left;
		if (inp->left) inp->left->right = newd;
		inp->left = newd;
		return 2;
	}
	// 3else @.@
	// "a" "+++*" "b", "a"-'++'-"+"-'*'-"b"
	nnode* newleft = zalc(sizeof(nnode));
	nnode* newright = zalc(sizeof(nnode));
	newleft->class = newright->class = tok_sym;
	newleft->row = newright->row = inp->row;
	newleft->col = inp->col;
	inp->col += idx;
	newright->col = inp->col + width;
	char* tmpaddr_mid = StrHeapN(inp->addr + idx, width);
	newright->addr = StrHeap(inp->addr + idx + width);// idx zo lenof left
	newleft->addr = inp->addr;
	newleft->addr[idx] = 0;
	inp->addr = tmpaddr_mid;
	if (newleft->left = inp->left) inp->left->right = newleft;
	if (newright->right = inp->right) inp->right->left = newright;
	newleft->right = newright->left = inp;
	inp->left = newleft;
	inp->right = newright;
	return 3;
}

dnode* NnodeToDnode(nnode* inp)
{
	if (sizeof(nnode) < sizeof(dnode)) return 0;
	nnode tmpn, *next, *crt = inp;
	dnode tmpd;
	while (crt)
	{
		next = crt->right;
		tmpn = *crt;
		tmpd.addr = crt->addr;
		tmpd.left = crt->left;
		tmpd.next = crt->right;
		tmpd.type = crt->class;
		MemCopyN(crt, &tmpd, sizeof tmpd);
		crt = next;
	}
	return (void*)inp;
}


