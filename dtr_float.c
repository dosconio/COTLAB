// ASCII TAB4 C99 ArnAssume
#include <stdio.h>
#include "cotlab.h"
#include <cdear.h>
#include <numar.h>
#include "parser.h"
#include "dtrlib.h"

#ifdef _MSC_VER// for MSVC
#define ermsg ("Bad input of %s" __FUNCIDEN__ "().")
#else
#define ermsg (char*)__FUNCIDEN__
#endif

static inline dnode* DJump(dnode* const callinfo, coe* (*CoeF)(coe*), void (*NumF)(numa*))
{
	dnode* ret = zalcof(dnode);
	if (callinfo && callinfo->addr)
	{
		if (callinfo->type == dt_int)
			ArithExplicitCoversionRise(callinfo, dt_float);
		if (callinfo->type == dt_float)
		{
			ret->addr = (void*)CoeCpy((void*)callinfo->addr);
			CoeF((void*)ret->addr);
			ret->type = dt_float;
		}
		else if (callinfo->type == dt_num)
		{
			ret->addr = (void*)NumCpy((void*)callinfo->addr);
			NumF((void*)ret->addr);
			ret->type = dt_num;
		}
		else
		{
			memf(ret);
			erro(ermsg);
		}
	}
	else
	{
		memf(ret);
		erro(ermsg);
	}
	return ret;
}

dnode* DtrSin(dnode* const callinfo)
{
	return DJump(callinfo, CoeSin, NumSin);
}

dnode* DtrCos(dnode* const callinfo)
{
	return DJump(callinfo, CoeCos, NumCos);
}

dnode* DtrTan(dnode* const callinfo)
{
	return DJump(callinfo, CoeTan, NumTan);
}

dnode* DtrASin(dnode* const callinfo)
{
	return DJump(callinfo, CoeAsin, NumAsin);
}

dnode* DtrACos(dnode* const callinfo)
{
	return DJump(callinfo, CoeAcos, NumAcos);
}

dnode* DtrATan(dnode* const callinfo)
{
	return DJump(callinfo, CoeAtan, NumAtan);
}

dnode* DtrSinh(dnode* const callinfo)
{
	return DJump(callinfo, CoeSinh, NumSinh);
}

dnode* DtrCosh(dnode* const callinfo)
{
	return DJump(callinfo, CoeCosh, NumCosh);
}

dnode* DtrTanh(dnode* const callinfo)
{
	return DJump(callinfo, CoeTanh, NumTanh);
}

dnode* DtrASinh(dnode* const callinfo)
{
	return DJump(callinfo, CoeAsinh, NumAsinh);
}

dnode* DtrACosh(dnode* const callinfo)
{
	return DJump(callinfo, CoeAcosh, NumAcosh);
}

dnode* DtrATanh(dnode* const callinfo)
{
	return DJump(callinfo, CoeAtanh, NumAtanh);
}

