// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// ! inf/nan will influence CoeInt()
#include <stdio.h>
#include "cotlab.h"
#include <cdear.h>
#include "parser.h"

int cottask(int src, void* point);

dnode* Dtr_system(dnode* const callinfo)
{
	if (callinfo->type == dt_str && callinfo->addr)
		(void)system(callinfo->addr);// avoid -Wunused-result
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
	dnode* ret = 0;
	if (callright->type == dt_float)
	{
		InodeUpdate(inods[1], callinfo->addr, (void*)CoeCpy((void*)callright->addr), dt_float, 0x80, InodeReleaseTofreeElementCotlab);
		ret = zalcof(dnode);
		ret->addr = (void*)CoeCpy((void*)callright->addr);
	}
	else if (callright->type == tok_iden)
	{
		erro("Something wrong yo non-function identifier linkage.");
	}
	else if (callright->addr)
	{
		InodeUpdate(inods[1], callinfo->addr, StrHeap(callright->addr), callright->type, 0x80, InodeReleaseTofreeElementCotlab);
		ret = zalcof(dnode);
		ret->addr = StrHeap(callright->addr);
	}
	else
	{
		InodeUpdate(inods[1], callinfo->addr, 0, 0, 0x80, InodeReleaseTofreeElementCotlab);
		return 0;
	}
	if (ret)
	{
		ret->type = callright->type;
	}
	return ret;
}




dnode* DtrPREPOSI(dnode* const callinfo)
{
	coe* co = 0;
	dnode* ret = zalcof(dnode);
	if (callinfo->type == dt_float)
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		ret->addr = StrHeap(callinfo->addr);
	else erro("Bad type of positive operator.");
	ret->type = callinfo->type;
	return ret;
}

dnode* DtrPRENEGA(dnode* const callinfo)
{
	coe* co = 0;
	dnode* ret = zalcof(dnode);
	if (callinfo->type == dt_float)
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
	{
		ret->addr = StrHeap(callinfo->addr);
		ret->addr[0] = (ret->addr[0] == '+') ? '-' : '+';
	}
	else erro("Bad type of negative operator.");
	ret->type = callinfo->type;
	return ret;
}

dnode* DtrARIADD(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	dnode* ret = zalcof(dnode);
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_int && cright->type == dt_int)
	{
		ret->addr = ChrAdd(callinfo->addr, cright->addr);
		ret->type = dt_int;;
		return ret;
	}
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	CoeAdd(co, src);
	CoeDel(src);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARISUB(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	dnode* ret = zalcof(dnode);
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_int && cright->type == dt_int)
	{
		ret->addr = ChrSub(callinfo->addr, cright->addr);
		ret->type = dt_int;;
		return ret;
	}
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	CoeSub(co, src);
	CoeDel(src);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIMUL(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	dnode* ret = zalcof(dnode);
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_int && cright->type == dt_int)
	{
		ret->addr = ChrMul(callinfo->addr, cright->addr);
		ret->type = dt_int;;
		return ret;
	}
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	CoeMul(co, src);
	CoeDel(src);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIDIV(dnode* const callinfo)
{
	dnode* cright = callinfo->right;
	dnode* ret = zalcof(dnode);
	coe* co = 0, * src = 0;
	if (callinfo->type == dt_float)
		co = CoeCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_int)
		co = CoeNew(callinfo->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	if (cright->type == dt_float)
		src = CoeCpy((void*)cright->addr);
	else if (cright->type == dt_int)
		src = CoeNew(cright->addr, "+0", "+1");
	else erro((char*)__FUNCIDEN__);
	CoeDiv(co, src);
	CoeDel(src);
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
	//{} warn
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

dnode* DtrARIFACT(dnode* const callinfo)// [OUT int]
{
	char* retstr = 0;
	if (callinfo->type == dt_float)
	{
		warn("The number will be converted into \"int\" type for factorial arithmetic.");
		CoeInt((coe*)callinfo->addr);
		retstr = ChrFactorial(((coe*)callinfo->addr)->coff);
	}
	else if (callinfo->type == dt_int)
		retstr = ChrFactorial(callinfo->addr);
	else erro("Bad type of factorial operator.");
	
	dnode* ret = zalcof(dnode);
	ret->addr = retstr;
	ret->type = dt_int;
	return ret;
}
