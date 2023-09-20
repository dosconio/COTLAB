// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

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

dnode* DtrPREPOSI(dnode* const callinfo)
{
	coe* co = CoeCpy((void*)callinfo->addr);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrPRENEGA(dnode* const callinfo)
{
	coe* co = CoeCpy((void*)callinfo->addr);
	co->coff[0] = (co->coff[0] == '-') ? '+' : '-';
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIADD(dnode* const callinfo)
{
	coe* co = CoeCpy((void*)callinfo->addr);
	CoeAdd(co, (void*)callinfo->next->addr);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARISUB(dnode* const callinfo)
{
	coe* co = CoeCpy((void*)callinfo->addr);
	CoeSub(co, (void*)callinfo->next->addr);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIMUL(dnode* const callinfo)
{
	coe* co = CoeCpy((void*)callinfo->addr);
	CoeMul(co, (void*)callinfo->next->addr);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}

dnode* DtrARIDIV(dnode* const callinfo)
{
	coe* co = CoeCpy((void*)callinfo->addr);
	CoeDiv(co, (void*)callinfo->next->addr);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)co;
	ret->type = tok_number;
	return ret;
}
