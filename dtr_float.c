// ASCII TAB4 C99 ArnAssume
#include <stdio.h>
#include "cotlab.h"
#include <cdear.h>
#include "parser.h"

#ifdef _MSC_VER// for MSVC
#define ermsg ("Bad input of %s" __FUNCIDEN__ "().")
#else
#define ermsg (char*)__FUNCIDEN__
#endif

static inline dnode* DJump(dnode* const callinfo, coe* (*CoeF)(coe*))
{
	coe* co = 0;
	if (callinfo && callinfo->addr)
	{
		if (callinfo->type == dt_float)
			co = CoeCpy((void*)callinfo->addr);
		else if (callinfo->type == dt_int)
			co = CoeNew(callinfo->addr, "+0", "+1");
	}
	else erro(ermsg);
	dnode* ret = zalcof(dnode);
	ret->addr = (void*)CoeF(co);
	ret->type = dt_float;
	return ret;
}

dnode* DtrSin(dnode* const callinfo)
{
	return DJump(callinfo, CoeSin);
}

dnode* DtrCos(dnode* const callinfo)
{
	return DJump(callinfo, CoeCos);
}

dnode* DtrTan(dnode* const callinfo)
{
	return DJump(callinfo, CoeTan);
}

dnode* DtrASin(dnode* const callinfo)
{
	return DJump(callinfo, CoeAsin);
}

dnode* DtrACos(dnode* const callinfo)
{
	return DJump(callinfo, CoeAcos);
}

dnode* DtrATan(dnode* const callinfo)
{
	return DJump(callinfo, CoeAtan);
}

dnode* DtrSinh(dnode* const callinfo)
{
	return DJump(callinfo, CoeSinh);
}

dnode* DtrCosh(dnode* const callinfo)
{
	return DJump(callinfo, CoeCosh);
}

dnode* DtrTanh(dnode* const callinfo)
{
	return DJump(callinfo, CoeTanh);
}

dnode* DtrASinh(dnode* const callinfo)
{
	return DJump(callinfo, CoeAsinh);
}

dnode* DtrACosh(dnode* const callinfo)
{
	return DJump(callinfo, CoeAcosh);
}

dnode* DtrATanh(dnode* const callinfo)
{
	return DJump(callinfo, CoeAtanh);
}

