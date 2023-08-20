// ASCII
#include <ustring.h>
#include <aldbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"
#include "cotlab.h"

const char
* OperatorSuff[] = { "++","SUFADD","--","SUFSUB",".","SUFMEMB","->","SUFMEMB",
	"[\1]","ARRSSC","{\1}", "OBRACE" },// "a[b]c => ARRSSC(a,b)c"
* OperatorPref[] = { "++","PREADD","--","PRESUB","+","PREPOSI","-","PRENEGA",
	"!","LOGNOT","~","BITNOT","*","PREMEMB","&","PREADDR", "\0sizeof", "SIZEOF" },
* OperatorLR1[] = { "*","ARIMUL","/","ARIDIV","%","ARIREM" },
* OperatorLR2[] = { "+","ARIADD","-","ARISUB" },
* OperatorLR3[] = { "<<","BITSHL",">>","BITSHR" },
* OperatorLR4[] = { "<","JBELOW","<=","JBEEQU",">","JGREAT",">=","JGREQU" },
* OperatorLR5[] = { "==","JEQUAL","!=","JNOTEQ" },
* OperatorLR6[] = { "&","BITAND" },
* OperatorLR7[] = { "^","BITXOR" },
* OperatorLR8[] = { "|","BITWOR" },
* OperatorLR9[] = { "&&","LOGAND" },
* OperatorLRA[] = { "||","LOGIOR" },
* OperatorRLL[] = { "\2?\2:\2","TERNAR" },//TERNAR(,,)
* OperatorRL[] = { "=","ASSIGN","+=","AGNSUM","-=","AGNDIF",
	"*=","AGNPRO","/=","AGNQUO","%=","AGNREM",
	"<<+","AGNSHL",">>=","AGNSHR","&=","AGNAND","^=","AGNXOR","|=","ASGNOR" },
	* OPTOREX1[] = { "\2if\2:\2","BLKJDG" }//
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

expr* TmpParsAriadd(fstruc* const fpara)
{
	// ret = p0 + p1
	if (fpara->counts == 2)
	{
		// assert paras[x].type == tok_number
		expr* result = zalc(sizeof(expr));
		result->ptype = dt_uint;
		result->content = (void*)((size_t)fpara->paras[0].content + (size_t)fpara->paras[1].content);
		return result;
	}
	return 0;
}

token* TokNew(char* content, token* left, token* right, enum datatype tt, size_t nest)// TODO
{
	token* tok = zalc(sizeof * tok);
	tok->addr = content;
	tok->left = left;
	tok->right = right;
	tok->type = tt;
	return tok;
}

void StrTokenParseElseEcho(iev* occupy, unsigned nest)
{
	unsigned tabs = nest + 1;
	for (unsigned i = 0; i < tabs;i++) putchar('\t');
	printf("Here is the else block:\n");
	Tode* crt = occupy->subfirst;
	while (crt)
	{
		if (crt->type == tok_else)
		{
			StrTokenParseElseEcho((iev*)crt->addr, nest + 1);
		}
		else
		{
			for (unsigned i = 0; i < tabs; i++) putchar('\t');
			StrTokenPrint(crt);
		}
		crt = crt->next;
	}
}

static void IevClear(iev* ie)
{
	memfree(ie->fname);
	Tode* crtt = ie->subfirst;
	while (crtt)
	{
		memfree(crtt->addr);
		crtt = crtt->next;
	}
}

void StrTokenLinkage(Tode* inp)
{
	// a tode -> a iev/fs -> a tode -> ...
	Tode* crt = inp;
	while (crt)
	{
		if (crt->type == tok_else)
		{
			iev* ie = (void*)crt->addr;
			fstruc* fs;
			if (!StrCompare(ie->fname, "ARIADD") && ie->subfirst && ie->subfirst->next && ie->subfirst->next->next && *ie->subfirst->next->next->addr)
			{
				Tode* subcrt = ie->subfirst;
				while (subcrt)
				{
					if (subcrt->type == tok_else) StrTokenLinkage(subcrt);
					subcrt = subcrt->next;
				}
				fs = zalc(sizeof(fstruc));
				fs->iden = StrHeap(ie->fname);
				fs->flink = TmpParsAriadd;
				fs->counts = 2;
				fs->paras = zalc(2 * sizeof(fstruc));
				fs->paras[0].ptype = (ie->subfirst->type == tok_else ? dt_func : dt_uint);
				fs->paras[1].ptype = (ie->subfirst->next->next->type == tok_else ? dt_func : dt_uint);
				// temp: no use heap
				if (ie->subfirst->type == tok_number)
					fs->paras[0].content = (void*)atoins(ie->subfirst->addr);
				else fs->paras[0].content = ie->subfirst;// input Tode
				if (ie->subfirst->next->next->type == tok_number)
					fs->paras[1].content = (void*)atoins(ie->subfirst->next->next->addr);
				else fs->paras[1].content = ie->subfirst->next->next;
				crt->addr = (void*)fs;
				//IevClear(ie);// {} forbid do this
			}
		}
		crt = crt->next;
	}
}

void StrTokenParse(Tode* inp)
{
	// origin from Haruno yo RFT27, principle of "Every action is a function, every object is in memory."
	// RFB19 Rewrite
	if (!inp) return;
	Tode* crt = inp;// The first is a occupy in tok_any
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
	// Convert all operators into function calling form;
	//
	crt = inp->next;// escape header ANY
	if (crt->next) crt = crt->next;//{} just and not only for "+"
	while (crt)// rest tok_type{any, v:str, v:num, sym, v:iden, spa}
	{
		// just for arithmetic "+"
		// ANY/SYM/SPA/NL STR/NUM/IDEN/ELSE(1) SYM(+) STR/NUM/IDEN/ELSE(2) SYM/SPA/NL/NULL
		// "ELSE" is only for the block that has processed and will go a function-calling-form as a value
		unsigned crtt = crt->type, leftt = crt->left->type;
		unsigned lleftt = crt->left->left->type;//{} just and not only for "+"
		if (crtt = tok_sym &&
			(leftt == tok_string || leftt == tok_number || leftt == tok_iden || leftt == tok_else) &&// bound to have the left
			(lleftt == tok_any || lleftt == tok_sym || lleftt == tok_space || crt->left->left->row < crt->row))
		{
			if (crt->next)
			{
				unsigned rightt = crt->next->type;
				if ((rightt == tok_string || rightt == tok_number || rightt == tok_iden || leftt == tok_else) &&
					(!crt->next->next || crt->next->next->type == tok_sym || crt->next->next->type == tok_space || crt->next->next->row > crt->row) &&
					crt->left->row == crt->row && crt->right->row == crt->row)
				{
					// create the structure in function calling form ---- IEV
					if (!StrCompare(crt->addr, "+"))
					{
						// 1+2 --> ARIADD(1,2), 1+2+3 --> ARIADD(ARIADD(1,2),3)
						iev* ieval = zalc(sizeof(iev));
						Tode* occupy = zalc(sizeof(occupy));
						occupy->addr = (void*)ieval;
						occupy->type = tok_else;
						occupy->row = crt->row;
						occupy->col = crt->left->col;
						ieval->fname = StrHeap("ARIADD");
						ieval->subfirst = crt->left;
						occupy->left = crt->left->left; crt->left->left = 0;
						occupy->right = crt->right->right; crt->right->right = 0;
						occupy->left->right = occupy;
						if (occupy->right)
							occupy->right->left = occupy;
						// need not change the symbol +
						crt = occupy->right;
						continue;
					}
				}
			}
		}
		crt = crt->next;
	}
	//
	// Check that each line only has either one IEV or one NUM/STR/IDEN, or it will be bad;
	//
	crt = inp->next;
	inp->row = 0x1;
	while (crt)
	{
		if (crt->row == crt->left->row)
		{
			printf("Failure of parsing on the line %llu.", 1 + crt->row);
			return;// {TODO} Free Process
		}
		crt = crt->next;
	}
	//
	// Echo for debug;
	//
	crt = inp->next;
	while (crt)
	{
		if (crt->type == tok_else)
		{
			StrTokenParseElseEcho((iev*)crt->addr, 0);
		}
		else
		{
			StrTokenPrint(crt);
		}
		crt = crt->next;
	}
	//
	// Convert each IEV to function-calling-form and make linkage;
	// [NESTED]
	StrTokenLinkage(inp);
	//
	// Make EXEC chain;
	//
	crt = inp->next;
	while (crt)
	{
		// NEED NOT
		crt = crt->next;
	}
	return;// temp
}



// EOF
