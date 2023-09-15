// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
#include <alice.h>// duplicate with ulibex.ustring
#include "ulibex.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <cdear.h>
#include "parser.h"
#include "cotlab.h"
#include "cotlib.h"

// ---- ---- ---- ---- Operator List ---- ---- ---- ----
const char
* OperatorSuff[] = { "++","SUFADD","--","SUFSUB",".","SUFMEMB","->","SUFMEMB",
	"[\1]","ARRSSC","{\1}", "OBRACE" },// <0> "a[b]c => ARRSSC(a,b)c"
* OperatorPref[] = { "++","PREADD","--","PRESUB","+","PREPOSI","-","PRENEGA",
	"!","LOGNOT","~","BITNOT","*","PREMEMB","&","PREADDR", "\0sizeof", "SIZEOF" },// <1>
* OperatorLR1[] = { "*","ARIMUL","/","ARIDIV","%","ARIREM" },// <2>
* OperatorLR2[] = { "+","ARIADD","-","ARISUB" },// <3>
* OperatorLR3[] = { "<<","BITSHL",">>","BITSHR" },// <4>
* OperatorLR4[] = { "<","JBELOW","<=","JBEEQU",">","JGREAT",">=","JGREQU" },// <5>
* OperatorLR5[] = { "==","JEQUAL","!=","JNOTEQ" },// <6>
* OperatorLR6[] = { "&","BITAND" },// <7>
* OperatorLR7[] = { "^","BITXOR" },// <8>
* OperatorLR8[] = { "|","BITWOR" },// <9>
* OperatorLR9[] = { "&&","LOGAND" },// <10>
* OperatorLRA[] = { "||","LOGIOR" },// <11>
* OperatorRLL[] = { "\2?\2:\2","TERNAR" },// <12> TERNAR(,,)
* OperatorRL[] = { "=","ASSIGN","+=","AGNSUM","-=","AGNDIF",
	"*=","AGNPRO","/=","AGNQUO","%=","AGNREM",
	"<<+","AGNSHL",">>=","AGNSHR","&=","AGNAND","^=","AGNXOR","|=","ASGNOR" },// <13>
	* OPTOREX1[] = { "\2if\2:\2","BLKJDG" }// <14>
	// COMMA
;
const char** OperatorsList[] =
{
	OperatorSuff, OperatorPref,
	OperatorLR1,OperatorLR2,OperatorLR3,OperatorLR4,OperatorLR5,OperatorLR6,OperatorLR7,OperatorLR8,OperatorLR9,OperatorLRA,
	OperatorRLL,OperatorRL,
	OPTOREX1
};
unsigned char OperatorsOrder[] = { 0,1, 0,0,0,0,0,0,0,0,0,0, 1,1,1 };// 0 Left-Right

#define _OPERATOR_GROUP_LEVEL_MAX 3// <GOAL> 14

// ---- ---- File design: from the bottom to top 

void StrTokenNestLinkage(nnode* inp)
{
	const char* txt[] = { "PREPOSI","PRENEGA","ARIADD","ARISUB","ARIMUL","ARIDIV" };
	void* fns[] = { DtrPREPOSI, DtrPRENEGA, DtrARIADD, DtrARISUB, DtrARIMUL, DtrARIDIV };
	for (nnode* crt = inp; crt; crt = crt->right)
	{
		if (crt->subf)
			StrTokenNestLinkage(crt->subf);
		if ((crt->class == dt_func) && crt->addr)
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
				fprintf(stderr, "Unknown function %s.", crt->addr);
				erro("Linkage error.");
				return;
			}
		}
	}

}

// Return 1 if success, 0 for failure.
static int StrTokenNestParseOperator(nnode* inp, unsigned level)
{
	/* {temp}
		Lv1 +(PREPOSI) -(PRENEGA)
		Lv2 *(ARIMUL) /(ARIDIV)
		Lv3 +(ARIADD) -(ARISUB)
	*/
	// rest tok_type{any, v:str, v:num, sym, v:iden, spa, func}
	if (level < 1) level = 1;
	int exist_sym = 0;

	nnode* crt;
	// ANY/SYM/SPA/NL STR/NUM/IDEN/FUNC(1) SYM(+-*/) STR/NUM/IDEN/FUNC(2) SYM/SPA/NL/NULL
	// ANY/SYM/SPA/NL SYM(pre +-) STR/NUM/IDEN/FUNC(2) SYM/SPA/NL/NULL
	for (crt = inp;crt;crt = crt->right)
	{
		if (crt->subf && (crt->class == dt_func) && crt->flag)
			StrTokenNestParseOperator(crt->subf, 0);
	}
lup:;
	crt = inp;
	switch (level)
	{
	case 1: goto point_pref;
	case 2: goto point_muldiv;
	case 3: goto point_addsub;
	default: goto endolup;
	}
point_pref:
	exist_sym = 0;
	for (; crt; crt = crt->right)
	{
		if (crt->class != tok_sym) continue; else exist_sym = 1;
		if (!crt->right) break;
		
		unsigned leftt = crt->left ? crt->left->class :0;
		unsigned rightt = crt->right->class;
		unsigned rrightt = 0;
		if ((!leftt || leftt == tok_any || leftt == tok_sym || leftt == tok_space || crt->left->row < crt->row) && 
			(rightt == tok_string || rightt == tok_number || rightt == tok_iden || rightt == dt_func) &&
			(!crt->right->right || (rrightt = crt->right->right->class) == tok_sym || rrightt == tok_space || crt->right->right->row > crt->row) && crt->right->row == crt->row)
		{
			for (size_t i = 0;i < StrLength(crt->addr);i++) if (crt->addr[i] == '+'||crt->addr[i] == '-')
			{
				(void)NnodeSymbolsDivide(crt, 1, i);
				nnode* fn = crt;// + or -
				srs(fn->addr, StrHeap(crt->addr[i] == '+' ? "PREPOSI" : "PRENEGA"));
				fn->class = dt_func;
				fn->subf = crt->right;// caution order
				if (fn->right = crt->right->right) 
					fn->right->left = fn;
				fn->subf->left = fn->subf->right = 0;
			}
		}
	}
	goto endolup;
point_muldiv:
	exist_sym = 0;
	for (; crt; crt = crt->right)
	{
		if (crt->class != tok_sym) continue; else exist_sym = 1;
		if (!crt->left || !crt->right) break;
		
		unsigned leftt = crt->left->class;
		unsigned lleftt = crt->left->left->class;// {TODO} may noe exist!!!
		unsigned rightt = crt->right->class;
		unsigned rrightt = 0;
		if ((leftt == tok_string || leftt == tok_number || leftt == tok_iden || leftt == dt_func) &&// bound to have the left for 2-opt operator
			(lleftt == tok_any || lleftt == tok_sym || lleftt == tok_space || crt->left->left->row < crt->row) && crt->left->row == crt->row &&
			(rightt == tok_string || rightt == tok_number || rightt == tok_iden || rightt == dt_func) &&
			(!crt->right->right || (rrightt = crt->right->right->class) == tok_sym || rrightt == tok_space || crt->right->right->row > crt->row) && crt->right->row == crt->row)
		{
			for (size_t i = 0;i < StrLength(crt->addr);i++) if (crt->addr[i] == '*'||crt->addr[i] == '/')
			{
				(void)NnodeSymbolsDivide(crt, 1, i);
				nnode* fn = zalcof(nnode);
				fn->addr = StrHeap(crt->addr[i] == '*' ? "ARIMUL" : "ARIDIV");
				fn->class = dt_func;
				fn->col = crt->col;
				fn->row = crt->row;
				crt->left->right = crt->right;
				crt->right->left = crt->left;
				if (fn->left = crt->left->left) fn->left->right = fn;
				if (fn->right = crt->right->right) fn->right->left = fn;
				fn->subf = crt->left;
				fn->subf->left = 0;
				crt->right->right = 0;
				NnodeDelete(crt);
				crt = fn;
			}
		}
	}
	goto endolup;
point_addsub:
	exist_sym = 0;
	for (; crt; crt = crt->right)
	{
		if (crt->class != tok_sym) continue; else exist_sym = 1;
		if (!crt->left || !crt->right) break;
		
		unsigned leftt = crt->left->class;
		unsigned lleftt = crt->left->left->class;
		unsigned rightt = crt->right->class;
		unsigned rrightt = 0;
		if ((leftt == tok_string || leftt == tok_number || leftt == tok_iden || leftt == dt_func) &&// bound to have the left for 2-opt operator
			(lleftt == tok_any || lleftt == tok_sym || lleftt == tok_space || crt->left->left->row < crt->row) && crt->left->row == crt->row &&
			(rightt == tok_string || rightt == tok_number || rightt == tok_iden || rightt == dt_func) &&
			(!crt->right->right || (rrightt = crt->right->right->class) == tok_sym || rrightt == tok_space || crt->right->right->row > crt->row) && crt->right->row == crt->row)
		{
			for (size_t i = 0;i < StrLength(crt->addr);i++) if (crt->addr[i] == '+'||crt->addr[i] == '-')
			{
				// 1+2 --> ARIADD(1,2), 1+2+3 --> ARIADD(ARIADD(1,2),3)
				(void)NnodeSymbolsDivide(crt, 1, i);
				nnode* fn = zalcof(nnode);
				fn->addr = StrHeap(crt->addr[i] == '+' ? "ARIADD" : "ARISUB");
				fn->class = dt_func;
				fn->col = crt->col;
				fn->row = crt->row;
				crt->left->right = crt->right;
				crt->right->left = crt->left;
				if (fn->left = crt->left->left) fn->left->right = fn;
				if (fn->right = crt->right->right) fn->right->left = fn;
				fn->subf = crt->left;
				fn->subf->left = 0;
				crt->right->right = 0;
				NnodeDelete(crt);
				crt = fn;
			}
		}
	}
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
	int exist_sym = 0;
	// E.g. 0+func(0w0)
	//      0+[ ... ], [...]={func, {0}{w}{0}}
	while (crt)
	{
		if (crt->class == tok_sym)
		{
			char c;
			for0(i, StrLength(crt->addr))
			{
				c = crt->addr[i];
				if (c == '(')
				{
					crtnest++;
					if (crtnest == 1)
					{
						last_parens = crt;
						state = NnodeSymbolsDivide(crt, 1, i);
						if (crtnest == 1 && (state == 2 || state == 3)) exist_sym = 1;
						break;
					}
				}
				else if (c == ')')
				{
					// do not care only one item in the block
					if (!crtnest || !last_parens) goto enderro;
					if (crtnest == 1)
					{
						state = NnodeSymbolsDivide(crt, 1, i);
						if (state == 1 || state == 3) exist_sym = 1;
						if (crt == last_parens->right)
						{
							if (parent && parent->subf == last_parens) parent->subf = crt->right;
							if (last_parens->left)last_parens->left->right = crt->right;
							if (crt->right)crt->right->left = last_parens->left;
							nnode* parend = crt;
							crt = last_parens->left;
							NnodeDelete(last_parens);
							NnodeDelete(parend);
							break;
						}
						// if the function have an identifier, the token will be kept, or create a new nnode for anonymity.
						nnode* fn = last_parens->left;
						if (!(last_parens->left && last_parens->left->class == tok_iden && last_parens->left->right == last_parens->row))// anonymity
						{
							fn = zalcof(nnode);
							fn->row = last_parens->row;
							fn->col = last_parens->col;
							fn->addr = 0;// {}{}{}{}
							if (fn->left = last_parens->left) last_parens->left->right = fn;
						}
						if (fn->right = crt->right) crt->right->left = fn;
						fn->subf = last_parens->right;
						fn->subf->left = 0;
						crt->left->right = 0;
						fn->class = dt_func;
						if (parent && parent->subf == last_parens) parent->subf = fn;
						NnodeDelete(last_parens);
						NnodeDelete(crt);
						crt = fn;
						crt->flag = 1;
						StrTokenNestParse(fn->subf, fn);
						exist_sym = 0;
						break;
					}
					crtnest--;
				}
				else if (crtnest == 1) exist_sym = 1;
			}
		}
		crt = crt->right;
		if (crt && (crt->row != crt->left->row)) last_parens = 0;
	}
	if (crtnest) return 1;
	StrTokenNestParseOperator(inp, 0);
	return 1;
enderro:
	fprintf(stderr, "Unmatched parenthesis at line %u" PRIuPTR ".", crt->row);
	erro("Fail at PARSER->StrTokenParse->StrTokenNestParse");
	return 0;
}

nnode* StrTokenParse(Tode* inp)
{
	// origin from Haruno yo RFT27, principle of "Every action is a function, every object is in memory."
	// RFB19, RFV13 Rewrite
	if (!inp) return 0;
	int state = 0;
	Tode* crt = inp;// The first is a occupy in tok_any
	nnode* nestok = 0, * crtnes = 0;
	//
	// Solve comment, Trim trailing or middle spaces;
	//
	while (inp && inp->type == tok_comment)
	{
		inp = inp->next;
		StrTokenThrow(inp->left);
	}
	crt = inp;
	while (crt)
	{
		if (crt->type == tok_comment ||
			crt->type == tok_space &&
			crt->row == crt->left->row &&
			(crt->row == crt->next->row || !crt->next))
			StrTokenThrow(crt);
		crt = crt->next;
	}
	//
	// String cat (must on a line);
	//
	crt = inp;
	while (crt)
	{
		while ((crt->next) && (crt->next->type == tok_string))
		{
			srs(crt->addr, StrHeapAppend(crt->addr, crt->next->addr));
			StrTokenThrow(crt->next);
		}
		crt = crt->next;
	}
	//
	// Discard any directive temporarily;
	//
	crt = inp;
	while (crt)
	{
		if (crt->type == tok_directive)
			StrTokenThrow(crt);
		crt = crt->next;
	}
	//
	// Make the imm-value live
	//
	CoeInit();
	for (crt = inp; crt; crt = crt->next)
	{
		// {TODO} covert tok_type to dt_type
		dt_float;
		if (crt->type == tok_number)
		{
			srs(crt->addr, CoeFromLocale(crt->addr));
		}
	}
	// ---- ---- ---- ---- LN ---> NS ---- ---- ---- ----
	// Restructure for nested
	// {WISH} UNISYM ADD SUPPORT FOR TOK-NEST BESIDE TOK-LINEAR
	//
	crt = inp; crtnes = nestok = zalc(sizeof(nnode));
	{
		crtnes->right = zalc(sizeof(nnode));
		crtnes->right->left = crtnes;
		crtnes->col = crt->col;
		crtnes->row = 1;// this of the next should be 0
		crtnes->addr = crt->addr;// directly use
		crtnes->class = crt->type;
		tnode* p = crt->next;
		memf(crt);
		crt = p;
	}
	while(crt)
	{
		crtnes = crtnes->right;
		crtnes->right = zalc(sizeof(nnode));
		crtnes->right->left = crtnes;
		crtnes->col = crt->col;
		crtnes->row = crt->row;
		crtnes->addr = crt->addr;// directly use
		crtnes->class = crt->type;
		tnode* p = crt;
		crt = crt->next;
		memf(p);
	}
	memf(crtnes->right);
	crtnes->right = 0;
	// Convert all operators into function calling form;
	state = StrTokenNestParse(nestok, 0);
	if (!state) return 0;// {TODO} erro
	// Check that each line only has one item;
	crtnes = nestok->right;
	nestok->row = 0x1;
	while (crtnes)
	{
		if (crtnes->row == crtnes->left->row)
		{
			printf("Failure of parsing on the line %llu.", 1 + crtnes->row);
			erro("Check that each line only has one item;");
			return 0;// {TODO} Free Process
		}
		crtnes = crtnes->right;
	}
	// Echo for debug;
#ifdef _dbg_echo
	NnodePrint(nestok->right, 0);
#endif
	// linkage;
	StrTokenNestLinkage(nestok);

	return nestok;// temp
}



// EOF
