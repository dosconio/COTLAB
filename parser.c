// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio

#include "cotlab.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <coear.h>
#include <numar.h>
#include "parser.h"
#include "dtrlib.h"

static size_t cabort_col, cabort_row;
static char* cabort_txt;



// ---- ---- File design: from the bottom to top 





// Return 1 for success, 0 for need to be released.
static int StrTokenNestLinkage(nnode* inp)
{
	int state = 1;
	const char* txt[] = { "PREPOSI","PRENEGA","ARIADD","ARISUB","ARIMUL","ARIDIV","ARIREM","ARIPOW","ARIFACT",
		"_dbg_test", "system", "load", "int", "ASSIGN",
		"sin", "cos", "tan", "asin","acos","atan","sinh", "cosh", "tanh", "asinh","acosh","atanh",
		
	};// Catious strcat here
	void* fns[] = { DtrPREPOSI, DtrPRENEGA, DtrARIADD, DtrARISUB, DtrARIMUL, DtrARIDIV, DtrARIREM, DtrARIPOW, DtrARIFACT,
		Dtr_dbg_test, Dtr_system, Dtr_load, Dtr_int,DtrASSIGN,
		DtrSin, DtrCos, DtrTan,DtrASin,DtrACos,DtrATan,DtrSinh, DtrCosh, DtrTanh,DtrASinh,DtrACosh,DtrATanh,
		
	};
	for (nnode* crt = inp; crt; crt = crt->next)
	{
		if (crt->subf)
		{
			state = StrTokenNestLinkage(crt->subf);
			if (!state) return state;
		}
		if ((crt->type == dt_func) && crt->addr)
		{
			for0(i, sizeof(txt) / sizeof(*txt))
			{
				if (!StrCompare(txt[i], crt->addr))
				{
					crt->bind = fns[i];
					break;
				}
			}
			if (!crt->bind)
			{
				cabort_row = crt->row;
				cabort_col = crt->col - (crt->addr ? StrLength(crt->addr) : 0);
				cabort_txt = StrHeap(crt->addr);
				//fprintf(stderr, "Unknown function %s(...): ", crt->addr);
				return 0;
			}
		}
	}
	return 1;
}

// Return 1 if success, 0 for failure.
// ANY/SYM/SPA/NL STR/NUM/IDEN/FUNC(2) SYM(suf !) SYM/SPA/NL/NULL
static int StrTokenNestParseOperatorSuffix(nnode** inp, nnode* parent, int* exist_sym)
{
	// SUF !
	nnode* crt = *inp;
	*exist_sym = 0;
	for (; crt; crt = crt->next)
	{
		if (crt->type != tok_symbol) continue; else *exist_sym = 1;
		if (!crt->left) break;

		char* idx;
		if (idx = (char*)StrIndexChars(crt->addr, "!"))
		{
			(void)NnodeSymbolsDivide(crt, 1, idx - crt->addr, parent);// {TODO} ACTIVE
			size_t lleftt = crt->left->left ? crt->left->left->type : tok_any;
			size_t leftt = crt->left->type;
			size_t rightt = crt->next ? crt->next->type : tok_any;
			//size_t rrightt = 0;

			if ((isidnsym(rightt) || crt->next->row != crt->row) &&
				isentity(leftt) &&
				(isidnsym(lleftt) || crt->left->left->row != crt->row) && crt->left->row == crt->row)
			{
				nnode* fn = crt;// + or -
				srs(fn->addr, StrHeap("ARIFACT"));// {TODO} ACTIVE
				fn->type = dt_func;
				if (crt->left == *inp) *inp = fn;
				NnodeBlock(fn, crt->left, crt->left, parent);// need not input parent
				crt = fn;
			}
		}
	}
	return 1;
}

// Return 1 if success, 0 for failure.
// ANY/SYM/SPA/NL SYM(pre +-) STR/NUM/IDEN/FUNC(2) SYM/SPA/NL/NULL
static int StrTokenNestParseOperatorPrefix(nnode* inp, nnode* parent, int* exist_sym)
{
	// PRE + -
	nnode* crt = inp;
	*exist_sym = 0;
	while (crt->next) crt = crt->next;
	for (; crt; crt = crt->left)
	{
		if (crt->type != tok_symbol) continue; else *exist_sym = 1;
		if (!crt->next) break;
		
		char* idx;
		if (idx = (char*)StrIndexCharsRight(crt->addr, "+-"))
		{
			(void)NnodeSymbolsDivide(crt, 1, idx - crt->addr, parent);// {TODO} ACTIVE
			size_t leftt = crt->left ? crt->left->type : tok_any;
			size_t rightt = crt->next->type;
			size_t rrightt = crt->next->next ? crt->next->next->type :tok_any;
			
			if ((isidnsym(leftt) || crt->left->row < crt->row) &&
				isentity(rightt) &&
				(isidnsym(rrightt) || crt->next->next->row > crt->row) && crt->next->row == crt->row)
			{
				nnode* fn = crt;// + or -
				srs(fn->addr, StrHeap(*idx == '+' ? "PREPOSI" : "PRENEGA"));// {TODO} ACTIVE
				fn->type = dt_func;
				NnodeBlock(fn, crt->next, crt->next, 0);// need not input parent
				crt = fn;
			}
		}
	}
	return 1;
}

static const char* StrTokenNestParseOperatorGetIdenByChar(const char** lst, size_t times, char op)
{
	for (size_t i = 0; i < times; i++)
	{
		if (lst[i << 1][0] == op && !lst[i << 1][1]) return lst[(i << 1) + 1];
	}
	return 0;
}

// Return 1 if success, 0 for failure.
// ANY/SYM/SPA/NL STR/NUM/IDEN/FUNC(1) SYM(+-*/) STR/NUM/IDEN/FUNC(2) SYM/SPA/NL/NULL
static int StrTokenNestParseOperatorRL(nnode** inp, nnode* parent, int* exist_sym, unsigned rank)
{
	// RL =
	nnode* crt = *inp;
	if (rank) return 0;
	const char* symlst = (const char* []){ "=" } [rank] ;
	const char** idens = (const char** []){ OperatorRL } [rank] ;
	size_t symnum = (size_t[]){ sizeof(OperatorRL) / sizeof(*OperatorRL) / 2 } [rank] ;
	*exist_sym = 0;
	while (crt->next) crt = crt->next;
	for (; crt; crt = crt->left)
	{
		if (crt->type != tok_symbol) continue; else *exist_sym = 1;
		if (!crt->left || !crt->next) break;
		char* idx;
		if (idx = (char*)StrIndexCharsRight(crt->addr, "="))
		{
			(void)NnodeSymbolsDivide(crt, 1, idx - crt->addr, parent);
			nnode* opleft = crt->left;
			nnode* opright = crt->next;
			size_t leftt = crt->left->type;
			size_t lleftt = crt->left->left ? crt->left->left->type : tok_any;// o zo tok_EOF
			size_t rightt = crt->next->type;
			size_t rrightt = crt->next->next ? crt->next->next->type : tok_any;
			if (isentity(leftt) &&// bound to have the left for 2-opt operator
				(isidnsym(lleftt) || crt->left->left->row < crt->row) && crt->left->row == crt->row &&
				isentity(rightt) &&
				(isidnsym(rrightt) || crt->next->next->row > crt->row) && crt->next->row == crt->row)
			{
				nnode* fn = NnodeInsert(crt->left, 0, parent);
				fn->addr = StrHeap(StrTokenNestParseOperatorGetIdenByChar(idens, symnum, *idx));
				fn->type = dt_func;
				if (crt->left == *inp) *inp = fn;// {HERE}
				NnodeRelease(crt, parent, NnodeReleaseTofreeCotlab);
				NnodeBlock(fn, opleft, opright, parent);
				crt = fn;
			}
		}
	}
	return 1;
}

// Return 1 if success, 0 for failure.
// ANY/SYM/SPA/NL STR/NUM/IDEN/FUNC(1) SYM(+-*/) STR/NUM/IDEN/FUNC(2) SYM/SPA/NL/NULL
static int StrTokenNestParseOperatorLR(nnode** inp, nnode* parent, int* exist_sym, unsigned rank)
{
	// LR0 ^{TODO}
	// LR1 * / %{TODO}
	// LR2 + -
	if (rank > 2) return 0;
	const char* symlst = (const char* []){ "^","*/%","+-" } [rank] ;
	const char** idens = (const char** []){ OperatorLR0,OperatorLR1,OperatorLR2 } [rank] ;
	size_t symnum = (size_t[]){ sizeof(OperatorLR0) / sizeof(*OperatorLR0) / 2,
	 sizeof(OperatorLR1) / sizeof(*OperatorLR1) / 2,
	 sizeof(OperatorLR2) / sizeof(*OperatorLR2) / 2 } [rank] ;
	*exist_sym = 0;
	for (nnode* crt = *inp; crt; crt = crt->next)
	{
		if (crt->type != tok_symbol) continue; else *exist_sym = 1;
		if (!crt->left || !crt->next) break;
		
		for (size_t i = 0; i < StrLength(crt->addr); i++) if (StrIndexChar(symlst, crt->addr[i]))// single operator
		{
			(void)NnodeSymbolsDivide(crt, 1, i, parent);
			nnode* opleft = crt->left;
			nnode* opright = crt->next;
			size_t leftt = crt->left->type;
			size_t lleftt = crt->left->left ? crt->left->left->type : tok_any;// o zo tok_EOF
			size_t rightt = crt->next->type;
			size_t rrightt = crt->next->next ? crt->next->next->type : tok_any;
			if (isentity(leftt) &&// bound to have the left for 2-opt operator
				(isidnsym(lleftt) || crt->left->left->row < crt->row) && crt->left->row == crt->row &&
				isentity(rightt) &&
				(isidnsym(rrightt) || crt->next->next->row > crt->row) && crt->next->row == crt->row)
			{
				nnode* fn = NnodeInsert(crt->left, 0, parent);
				fn->addr = StrHeap(StrTokenNestParseOperatorGetIdenByChar(idens, symnum, crt->addr[i]));
				fn->type = dt_func;
				if (crt->left == *inp) *inp = fn;
				NnodeRelease(crt, parent, NnodeReleaseTofreeCotlab);
				NnodeBlock(fn, opleft, opright, parent);
				crt = fn;
				break;
			}
		}
	}
	return 1;
}

// Return 1 if success, 0 for failure.
static int StrTokenNestParseOperator(nnode* inp, nnode* parent, unsigned level)
{
	// rest tok_type{any, v:str, v:num, sym, v:iden, spa, func}
	int exist_sym = 0, state = 0;
	if (!inp) return 1;
	
	// Nested call by StrTokenNestParse()
lup:;
	switch (level)
	{
	case 0: goto point_unary;// pref and suff
	case 1: goto point_pow;
	case 2: goto point_muldiv;// besides rem
	case 3: goto point_addsub;
	case 4: goto point_assign;
	default: goto endolup;
	}
point_unary:
	state = StrTokenNestParseOperatorSuffix(&inp, parent, &exist_sym);
	if (!state) return state;// return by eax
	state = StrTokenNestParseOperatorPrefix(inp, parent, &exist_sym);
	if (!state) return state;// return by eax
	goto endolup;
point_pow:
	state = StrTokenNestParseOperatorLR(&inp, parent, &exist_sym, 0);
	if (!state) return state;// return by eax
	goto endolup;
point_muldiv:
	state = StrTokenNestParseOperatorLR(&inp, parent, &exist_sym, 1);
	if (!state) return state;// return by eax
	goto endolup;
point_addsub:// keep similar with muldiv now
	state = StrTokenNestParseOperatorLR(&inp, parent, &exist_sym, 2);
	if (!state) return state;// return by eax
	goto endolup;
point_assign:// keep similar with muldiv now
	state = StrTokenNestParseOperatorRL(&inp, parent, &exist_sym, 0);
	if (!state) return state;// return by eax
	goto endolup;
endolup:
	if (!exist_sym) return 1;
	if (++level > _OPERATOR_GROUP_LEVEL_MAX) return 1;
	goto lup;
	// seek other symbol and turn to next level
	//     if some kept at the end, an exception will be put.

}

// Return 1 if success, 0 for failure.. All multi-token to function form, so leave the single token such as NUM and so on.
static int StrTokenNestParse(nnode* inp, nnode* parent)
{
	//- for parens and parend
	nnode* crt = inp;
	size_t crtnest = 0;
	nnode* last_parens = 0;
	int state = 0;
	int exist_sym = 0;// {TODO}
	// E.g. 0+func(0w0)
	//      0+[ ... ], [...]={func, {0}{w}{0}}
	while (crt)
	{
		if (crt->type == tok_symbol)
		{
			char c;
			size_t count = StrLength(crt->addr);
			for0(i, count)
			{
				c = crt->addr[i];
				if (c == '(')
				{
					crtnest++;
					if (crtnest == 1)
					{
						last_parens = crt;
						state = NnodeSymbolsDivide(crt, 1, i, parent);
						if (crtnest == 1 && (state == 2 || state == 3)) exist_sym = 1;
						break;
					}
				}
				else if (c == ')')
				{
					// do not care only one item in the block
					if (!crtnest || !last_parens) goto enderro;
					crtnest--;
					if (crtnest == 0)
					{
						state = NnodeSymbolsDivide(crt, 1, i, parent);
						if (state == 1 || state == 3) exist_sym = 1;
						nnode* fn = last_parens->left;// assume not anonymity
						if (!(last_parens->left && last_parens->left->type == tok_identy && last_parens->left->row == last_parens->row))// anonymity
							fn = NnodeInsert(last_parens, 0, parent);
						NnodeBlock(fn, last_parens->next, crt->left, 0);// need not input parent
						fn->type = dt_func;
						if (last_parens == inp) 
							inp = fn;
						NnodeRelease(last_parens, parent, NnodeReleaseTofreeCotlab);
						NnodeRelease(crt, parent, NnodeReleaseTofreeCotlab);
						crt = fn;
						StrTokenNestParse(fn->subf, fn);
						/// exist_sym = 0; ¤Þ
						break;
					}
				}
			}
		}
		crt = crt->next;
		if (crt && (crt->row != crt->left->row)) last_parens = 0;
	}
	//{TODO} if (crtnest) erro("Match error");
	StrTokenNestParseOperator((parent && parent->subf) ? parent->subf : inp, parent, 0);
	return 1;
enderro:
	fprintf(stderr, "Unmatched parenthesis at line %" PRIuPTR ".", crt->row);
	erro("Fail at PARSER->StrTokenParse->StrTokenNestParse");
	return 0;
}

nnode* StrTokenParse(tode* inp)
{
	
	inp->row = inp->next ? inp->next->row + 1 : 0;
	tode* crt = inp;// The first is a occupy in tok_any
	nnode* nestok = 0, * crtnes = 0;



	// Make the imm-value live
	CoeInit();
	for (crt = inp; crt; crt = crt->next)
	{
		dt_float;
		if (crt->type == tok_number)
		{
			char* spanchr;
			if (spanchr = (char*)StrIndexChars(crt->addr, "ij"))
			{
				*spanchr = 0;
				crt->type = dt_num;
				coe* tmpcoe = CoeFromLocale(crt->addr);
				srs(crt->addr, NumNewComplex("+0", "+0", "+1", tmpcoe->coff, tmpcoe->expo, tmpcoe->divr));
				CoeDel(tmpcoe);
			}
			else if (StrIndexCharsExcept(crt->addr, "0123456789"))
			{
				srs(crt->addr, CoeFromLocale(crt->addr));
			}
			else// flat integer
			{
				crt->type = dt_int;
				srs(crt->addr, StrHeapAppend("+", crt->addr));
			}
		}
	}
	///

	///

	///



	// Convert all operators into function calling form;
	state = StrTokenNestParse(nestok, 0);
	if (!state) return 0;// {TODO} erro
	// Check that each line only has one item;
	if (nestok->next && (crtnes = nestok->next->next))while (crtnes)
	{
		if (crtnes->row == crtnes->left->row)
		{
			cabort_row = crtnes->row, cabort_col = crtnes->col - (crtnes->addr ? StrLength(crtnes->addr) : 0);
			NnodesRelease(nestok, 0, NnodeReleaseTofreeCotlab);
			cabort("Invalid expression.", cabort_row, cabort_col, 0);
			return 0;
		}
		crtnes = crtnes->next;
	}

	// linkage;
	// StrTokenNestVariable(nestok, 0); while executing
	state = StrTokenNestLinkage(nestok);
	if (!state)
	{
		NnodesRelease(nestok, 0, NnodeReleaseTofreeCotlab);
		cabort("Unknown identifier.", cabort_row, cabort_col, cabort_txt);
		cabort_txt = 0;
		return 0;
	}
	return nestok;// temp
}



// EOF
