// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
// ! inf/nan will influence CoeInt()
#include <stdio.h>
#include "cotlab.h"
#include <coear.h>
#include <numar.h>
#include "parser.h"

#define isaritype(x)(x==dt_int||x==dt_float||x==dt_num)

int cottask(int src, void* point);

void ArithExplicitCoversionRise(dnode* const a, size_t typ)
{
	if (!isaritype(a->type) || !a->addr) return;
	if (a->type == typ) return;
	if (a->type == dt_num);
	else if (a->type == dt_float)
	{
		if (typ == dt_num)
		{
			coe* tmpcoe = (void*)a->addr;
			a->addr = (void*)NumNewComplex(tmpcoe->coff, tmpcoe->expo, tmpcoe->divr, "+0", "+0", "+1");
			CoeDel(tmpcoe);
		}
		else if (typ == dt_int);
		a->type = typ;
	}
	else if (a->type == dt_int)
	{
		if (typ == dt_num)
		{
			srs(a->addr, NumNewComplex(a->addr, "+0", "+1", "+0", "+0", "+1"));
		}
		else if (typ == dt_float)
		{
			srs(a->addr, CoeNew(a->addr, "+0", "+1"));
		}
		a->type = typ;
	}
}

void ArithImplicitCoversion(dnode* const a, dnode* const b)
{
	if (!isaritype(a->type) || !isaritype(b->type)) return;
	if (a->type == b->type)return;
	if (a->type == dt_num || b->type == dt_num)
	{
		dnode* dest = a->type == dt_num ? b : a;
		if (dest->type == dt_int)
		{
			srs(dest->addr, NumNewComplex(dest->addr, "+0", "+1", "+0", "+0", "+1"));
		}
		else if (dest->type == dt_float)
		{
			coe* tmpcoe = (coe*)dest->addr;
			dest->addr = (void*)NumNewComplex(tmpcoe->coff, tmpcoe->expo, tmpcoe->divr, "+0", "+0", "+1");
			CoeDel(tmpcoe);
		}
		dest->type = dt_num;
	}
	else if (a->type == dt_float || b->type == dt_float)
	{
		dnode* dest = a->type == dt_float ? b : a;
		if (dest->type == dt_int)
		{
			srs(dest->addr, CoeNew(dest->addr, "+0", "+1"));
		}
		dest->type = dt_float;
	}
}

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
	if (!callinfo || callinfo->type != tok_identy || !callinfo->addr ||
		!(callright = callinfo->next)) return 0;
	coe* co = 0;
	inode* crt = 0;
	dnode* ret = 0;
	if (callright->type == dt_float)
	{
		if (!InodeUpdate(inods[1], callinfo->addr, (void*)CoeCpy((void*)callright->addr), callright->type, 0x80, InodeReleaseTofreeElementCotlab))
		{
			warn("The assignment is invalid.");
			return 0;
		}
		ret = zalcof(dnode);
		ret->addr = (void*)CoeCpy((void*)callright->addr);
	}
	else if (callright->type == dt_num)
	{
		if (!InodeUpdate(inods[1], callinfo->addr, (void*)NumCpy((void*)callright->addr), callright->type, 0x80, InodeReleaseTofreeElementCotlab))
		{
			warn("The assignment is invalid.");
			return 0;
		}
		ret = zalcof(dnode);
		ret->addr = (void*)NumCpy((void*)callright->addr);
	}
	else if (callright->type == tok_identy)
	{
		erro("Something wrong yo non-function identifier linkage.");
	}
	else if (callright->addr)
	{
		if (!InodeUpdate(inods[1], callinfo->addr, StrHeap(callright->addr), callright->type, 0x80, InodeReleaseTofreeElementCotlab))
		{
			warn("The assignment is invalid.");
			return 0;
		}
		ret = zalcof(dnode);
		ret->addr = StrHeap(callright->addr);
	}
	else
	{
		if (!InodeUpdate(inods[1], callinfo->addr, 0, 0, 0x80, InodeReleaseTofreeElementCotlab))
		{
			warn("The assignment is invalid.");
			return 0;
		}
		return 0;
	}
	if (ret)
	{
		ret->type = callright->type;
	}
	return ret;
}

// ---- ---- ---- ----

dnode* DtrPREPOSI(dnode* const callinfo)
{
	coe* co = 0;
	dnode* ret = zalcof(dnode);
	if (callinfo->type == dt_num)
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
	else if (callinfo->type == dt_float)
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
	if (callinfo->type == dt_num)
	{
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
		CoeNeg(&((numa*)ret->addr)->Real);
		CoeNeg(&((numa*)ret->addr)->Imag);
	}
	else if (callinfo->type == dt_float)
	{
		co = CoeCpy((void*)callinfo->addr);
		co->coff[0] = (co->coff[0] == '+') ? '-' : '+';
		ret->addr = (void*)co;
	}
	else if (callinfo->type == dt_int)
	{
		ret->addr = StrHeap(callinfo->addr);
		((char*)ret->addr)[0] = (((char*)ret->addr)[0] == '+') ? '-' : '+';
	}
	else erro("Bad type of negative operator.");
	ret->type = callinfo->type;
	return ret;
}

dnode* DtrARIADD(dnode* const callinfo)
{
	dnode* cright = callinfo->next;
	dnode* ret = zalcof(dnode);
	if (callinfo->type == dt_str && cright->type == dt_str)
	{
		ret->addr = StrHeapAppend(callinfo->addr, cright->addr);
		ret->type = dt_str;
		return ret;
	}
	ArithImplicitCoversion(callinfo, cright);
	if (callinfo->type == dt_int && cright->type == dt_int)
	{
		ret->addr = ChrAdd(callinfo->addr, cright->addr);
	}
	else if (callinfo->type == dt_float && cright->type == dt_float)
	{
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
		CoeAdd((void*)ret->addr, (void*)cright->addr);
	}
	else if (callinfo->type == dt_num && cright->type == dt_num)
	{
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
		NumAdd((void*)ret->addr, (void*)cright->addr);
	}
	else
	{
		memf(ret);
		erro((char*)__FUNCIDEN__);
		return 0;
	}
	ret->type = callinfo->type;
	return ret;
}

dnode* DtrARISUB(dnode* const callinfo)
{
	dnode* cright = callinfo->next;
	dnode* ret = zalcof(dnode);
	ArithImplicitCoversion(callinfo, cright);
	if (callinfo->type == dt_int && cright->type == dt_int)
	{
		ret->addr = ChrSub(callinfo->addr, cright->addr);
	}
	else if (callinfo->type == dt_float && cright->type == dt_float)
	{
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
		CoeSub((void*)ret->addr, (void*)cright->addr);
	}
	else if (callinfo->type == dt_num && cright->type == dt_num)
	{
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
		NumSub((void*)ret->addr, (void*)cright->addr);
	}
	else
	{
		memf(ret);
		erro((char*)__FUNCIDEN__);
		return 0;
	}
	ret->type = callinfo->type;
	return ret;
}

dnode* DtrARIMUL(dnode* const callinfo)
{
	dnode* cright = callinfo->next;
	dnode* ret = zalcof(dnode);
	ArithImplicitCoversion(callinfo, cright);
	if (callinfo->type == dt_str && cright->type == dt_int)
	{
		ret->addr = StrHeap("");
		while (((char*)cright->addr)[1] != '0')
		{
			srs(ret->addr, StrHeapAppend(ret->addr, callinfo->addr));
			srs(cright->addr, ChrSub(cright->addr, "+1"));
		}
		ret->type = dt_str;
		return ret;
	}
	if (callinfo->type == dt_int && cright->type == dt_int)
	{
		ret->addr = ChrMul(callinfo->addr, cright->addr);
	}
	else if (callinfo->type == dt_float && cright->type == dt_float)
	{
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
		CoeMul((void*)ret->addr, (void*)cright->addr);
	}
	else if (callinfo->type == dt_num && cright->type == dt_num)
	{
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
		NumMul((void*)ret->addr, (void*)cright->addr);
	}
	else
	{
		memf(ret);
		erro((char*)__FUNCIDEN__);
		return 0;
	}
	ret->type = callinfo->type;
	return ret;
}

dnode* DtrARIDIV(dnode* const callinfo)
{
	dnode* cright = callinfo->next;
	dnode* ret = zalcof(dnode);
	if (!isaritype(callinfo->type) || !isaritype(cright->type))
	{
		memf(ret);
		erro((char*)__FUNCIDEN__);
		return 0;
	}
	if (callinfo->type == dt_num || cright->type == dt_num)
	{
		ArithImplicitCoversion(callinfo, cright);
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
		NumDiv((void*)ret->addr, (void*)cright->addr);
		ret->type = dt_num;
	}
	else
	{
		ArithExplicitCoversionRise(callinfo, dt_float);
		ArithImplicitCoversion(callinfo, cright);
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
		CoeDiv((void*)ret->addr, (void*)cright->addr);
		ret->type = dt_float;
	}
	return ret;
}

dnode* DtrARIREM(dnode* const callinfo)// [OUT INT]
{
	// (int, int)
	// There is implicit type conversion
	dnode* cright = callinfo->next;
	if (callinfo->type != dt_float && callinfo->type != dt_int) erro("Bad dest op of rem.");
	if (callinfo->next->type != dt_float && callinfo->next->type != dt_int) erro("Bad sors op of rem.");
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
	dnode* cright = callinfo->next;
	dnode* ret = zalcof(dnode);
	if (!isaritype(callinfo->type) || !isaritype(cright->type))
	{
		memf(ret);
		erro((char*)__FUNCIDEN__);
		return 0;
	}
	if (callinfo->type == dt_num || cright->type == dt_num || (callinfo->type == dt_float && ((coe*)callinfo->addr)->coff[0] == '-') || (callinfo->type == dt_int && ((char*)callinfo->addr)[0] == '-'))
	{
		ArithExplicitCoversionRise(callinfo, dt_num);
		ArithExplicitCoversionRise(cright, dt_num);
		ret->addr = (void*)NumCpy((void*)callinfo->addr);
		NumPow((void*)ret->addr, (void*)cright->addr);
		ret->type = dt_num;
	}
	else
	{
		ArithExplicitCoversionRise(callinfo, dt_float);
		ArithImplicitCoversion(callinfo, cright);
		ret->addr = (void*)CoeCpy((void*)callinfo->addr);
		CoePow((void*)ret->addr, (void*)cright->addr);
		ret->type = dt_float;
	}
	//{TODO} notnega_int ^ notnega_int = notnega_int
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
