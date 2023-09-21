// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

#pragma warning(disable:6011)// nullptr check
#include "ulibex.h"
#include <stdio.h>
#include <cdear.h>
#include "parser.h"

int cottask(int src, void* point);

dnode* Dtr_system(dnode* const callinfo)
{
	if (callinfo->type == dt_str && callinfo->addr)
		system(callinfo->addr);
	return 0;
}

dnode* Dtr_load(dnode* const callinfo)
{
	if (callinfo->type == dt_str && callinfo->addr)
		cottask(0, callinfo->addr);
	return 0;
}

dnode* Dtr_dbg_test(dnode* const callinfo)
{
	puts("Hello, Datura");
	coe* co = CoeNew("+1", "+1", "+1");
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* Dtr_int(dnode* const callinfo)
{
	if (!callinfo || !callinfo->addr) return 0;
	coe* co = 0;
	dnode* ret = 0;
	if (callinfo->type == dt_float)
	{
		ret = zalcof(dnode);
		co = CoeCpy((void*)callinfo->addr);
		CoeInt(co);
		char* cof = StrHeap(co->coff);
		CoeDel(co);
		ret->addr = cof;
		ret->type = dt_int;
	}
	else if (callinfo->type == dt_int)
	{
		ret = zalcof(dnode);
		char* cof = StrHeap(callinfo->addr);
		ret->addr = cof;
		ret->type = dt_int;
	}
	return ret;
}


dnode* DtrASSIGN(dnode* const callinfo)
{
	const dnode* callright = 0;
	if (!callinfo || callinfo->type != tok_iden || !callinfo->addr ||
		!(callright = callinfo->right)) return 0;
	coe* co = 0;
	inode* crt = 0;
	if (callright->type == dt_float)
	{
		InodeUpdate(inods[1], callinfo->addr, (void*)CoeCpy((void*)callright->addr), dt_float, 2, InodeReleaseTofreeElementCotlab);
	}
	else if (callright->type == tok_iden)
	{
		warn("TODO and ISSUE: Copy or copy pointer.");
	}
	else
	{
		InodeUpdate(inods[1], callinfo->addr, StrHeap(callright->addr), callright->type, 2, InodeReleaseTofreeElementCotlab);
	}
	return 0;
}




dnode* DtrPREPOSI(dnode* const callinfo)
{
	coe* co = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of positive operator.");
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrPRENEGA(dnode* const callinfo)
{
	coe* co = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of negative operator.");
	co->coff[0] = (co->coff[0] == '-') ? '+' : '-';
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIADD(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of add operator.");
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro("Bad type of add operator.");
	CoeAdd(co, src);
	CoeDel(src);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARISUB(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of sub operator.");
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro("Bad type of sub operator.");
	CoeSub(co, src);
	CoeDel(src);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIMUL(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of mul operator.");
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro("Bad type of mul operator.");
	CoeMul(co, src);
	CoeDel(src);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIDIV(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of div operator.");
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro("Bad type of div operator.");
	CoeDiv(co, src);
	CoeDel(src);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIREM(dnode* const callinfo)// [OUT INT]
{
	// (int, int)
	// There is implicit type conversion
	dnode* cright = callinfo->right;
	if (callinfo->type != dt_float && callinfo->type != dt_int) erro("Bad dest");
	if (callinfo->next->type != dt_float && callinfo->next->type != dt_int) erro("Bad sors");
	coe* des = callinfo->type == dt_float ? CoeCpy((void*)callinfo->addr) :
		CoeNew(callinfo->addr, "+0", "+1");
	coe* src = cright->type == dt_float ? CoeCpy((void*)cright->addr) :
		CoeNew(cright->addr, "+0", "+1");
	CoeInt(des);
	CoeInt(src);
	char* dest = StrHeap(des->coff);
	char* sors = StrHeap(src->coff);
	ChrDiv(dest, sors);
	dnode* ret = zalcof(dnode);
	memf(dest);
	ret->addr = sors;
	ret->type = dt_int;
	CoeDel(src);
	CoeDel(des);
	return ret;
}

dnode* DtrARIPOW(dnode* const callinfo)// [OUT CDE]
{
	warn("Power: Temporarily for integer exponent.");
	dnode* cright = callinfo->right;
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro("Bad type of power operator.");
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro("Bad type of power operator.");
	CoePow(co, src);
	CoeDel(src);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}
