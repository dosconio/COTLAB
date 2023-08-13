// GBK Codeset
#include <ustring.h>
#include <aldbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "cotlab.h"
#include "token.h"

//TODO. size_t pars_token_limit = 0x1000 * 0x1000;

static void StrTokenPrintAll(Tode* first)// TODO
{
	do printf("Token: [R%llu,C%llu][%s][%llu] %s\n", first->row, first->col, ((const char* []){"END", "ANY", "STR", "CMT", "DIR", "NUM", "SYM", "IDN", "SPC", "ELS"})[first->type], StrLength(first->addr), first->type == tok_space ? "" : first->addr);
	while (first = first->next);
}

// ---- ---- ---- ---- ---- ---- ---- ---- 

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
* OperatorRLL[] = { "?\1:\1","TERNAR" },
* OperatorRL[] = { "=","ASSIGN","+=","AGNSUM","-=","AGNDIF",
	"*=","AGNPRO","/=","AGNQUO","%=","AGNREM",
	"<<+","AGNSHL",">>=","AGNSHR","&=","AGNAND","^=","AGNXOR","|=","ASGNOR" },
	* OPTOREX1[] = { "if\1:\1","BLKJDG" }
	// COMMA
;// SYM NOTSYM SYM NOTSYM ...

const char** OperatorsList[] =
{
	OperatorSuff, OperatorPref,
	OperatorLR1,OperatorLR2,OperatorLR3,OperatorLR4,OperatorLR5,OperatorLR6,OperatorLR7,OperatorLR8,OperatorLR9,OperatorLRA,
	OperatorRLL,OperatorRL,
	OPTOREX1
};
unsigned char OperatorsOrder[] = { 0,1, 0,0,0,0,0,0,0,0,0,0, 1,1,0 };// 0 Left-Right


token* TokNew(char* content, token* parent, token* subf, token* left, token* right, TokType tt, size_t nest)// TODO
{
	token* tok;
	memalloc(tok, sizeof *tok);
	tok->tok_content = content;
	tok->parent = parent;
	tok->subfirst = subf;
	tok->left = left;
	tok->right = right;
	tok->value = 0;
	tok->toktype = tt;
	tok->address = 0;
	return tok;
}

//void TokRemove(token* to)
//{
//	if(to->left) to->left->right = to->right;
//	if(to->right) to->right->left = to->left;
//	if (to->parent && to->parent->subfirst == to)
//		to->parent->subfirst = 0;
//	if (to->subfirst);// TODO!!!!!!!!!!
//	memfree(to->tok_content);
//	memfree(to);
//}

static token* TokCpy(const token* sor)// TODO
{
	token* tok;
	memalloc(tok, sizeof tok);
	MemCopyN(tok, sor, sizeof tok);
	tok->tok_content = StrHeap(sor->tok_content);
	return tok;
}

//void TokScpClr(token* headtok)// TODO.
//{
//	token* crt = headtok;
//	memalloc(crt->left, 4);// Use time to get better struct
//	do
//	{
//		if (crt->subfirst) TokScpClr(crt->subfirst);
//		// if (crt->toktype == tok_anyexp && crt->sublast) TokScpClr(crt->sublast);
//		memfree(crt->left);
//		memfree(crt->tok_content);
//	} while (crt->right && (crt = crt->right));
//	memfree(crt);
//}

// RFT29: Objects are all l-value.
Todir* StrTokenStruct(Tode* in)// Tode -> Tree of directives
{
	/* created by Haruno yo RFT27, principle of "Every action is a function, every object is in memory."
		if a==b:
			a(); b();// 1-line, convert to comma expression
		[1] Preproc: symbol parser then solve sym_space, then check
			sym_num, sym_iden, sym_string and useful token
		if(equal(a,b),(a(),b()))
		[2] ordinate
		if { equal{ a b } { a {} b {} } }
		[3] directive bind to make Tree of directives (Todir)
		[-] IO and accept object changes
	*/
	if (!in) return 0;
	Tode* tokfirst = in;
	Tode* crt = tokfirst;
	Todir* resfirst, * rescrt;
	char chr;
	size_t crtnest = 0, crtline = 0;
	rescrt = resfirst = TokNew(StrHeap(""), 0, 0, 0, 0, crt->type, 0);
	// Comment Solve and keep only the space in the left of the line(row!>last.row and row=next.row).
	{
		Tode* crt = in, * slave = crt->next;
		do
		{
			slave = crt->next;
			if (crt->type == tok_comment)			
				StrTokenThrow(crt);
			else if (crt->type == tok_space &&
				crt->row == crt->left->row &&
				(crt->next && crt->row == crt->next->row || !crt->next))
			{
				StrTokenThrow(crt);
			}
		} while (crt = slave);
	}
	// String Cat
	{
		Tode* crt = in;
		do if (crt->type == tok_string)
		{
			while ((crt->next) && (crt->next->type == tok_string))
			{
				Tode* slave = crt->next;
				srs(crt->addr, StrHeapAppend(crt->addr, crt->next->addr));
				StrTokenThrow(slave);
			}
		} while (crt->next && (crt = crt->next));
	}
	// Convert non_iden[] non_iden{} into BRACKET() BRACEBR()
	{
		Tode* crt = in;
	}
	// opeartors parser for tok_symbol
	{
		Tode* crt = in;
		// try a+++b : [OperatorSuff]
		// try a.b+c.d
		// try a[b]+c[d]
		// try a ? b ? c : d : e 
		// try ++a++
		// try sin(1)+2*3

		

	}

	
	/*
	while (1)
	{
		// assert(crt->row >= crtline)
		if (crt->row > crtline)
		{
			crt->row = crtline;
		}
		//TODO. indent
		loop0:
		if (!crt->next) break;
		crt = crt->next;
		if (crt->type == tok_any || crt->type == tok_comment || crt->type == tok_else)
		{
			if (crt->next);
			else break;
			goto loop0;
		}
		else if (crt->type == tok_space)// not on the start of a line, omit!
		{
			size_t ExiNl = 0;
			for (size_t j = 0; chr = crt->addr[j]; j++)
			{
				if (chr == '\n' || chr == '\r')// seek the last NL character
				{
					if (crt->left && crt->left->len == tok_sym)
					{
						if (StrCharLast(crt->left->addr) == '\\')
						{
							if (!crt->left->left)
								erro("TokScp Parse: \\ Line feed error.");
							if (!crt->next) break;
							if (crt->left->addr[1] == 0)
							{
								token* tmpptr = rescrt;
								rescrt = rescrt->left;
								rescrt->right = 0;
								tmpptr->left = 0;
								TokScpClr(tmpptr);
							}
							else
							{
								rescrt->tok_content[StrLength(rescrt->tok_content) - 1] = 0;
							}
						}
						else ExiNl = j + 1;
					}
					else
						ExiNl = j + 1;
				}
			}
			if (!ExiNl)
			{
				goto loop0;
			}
			else
			{
				// ExiNl--;
				MemRelative(crt->addr + ExiNl,
						StrLength(crt->addr + ExiNl) + 1,
						-(ptrdiff_t)(ExiNl));
				// if (!rescrt->left)
				// 	erro("Parse ; Match");
				if (rescrt->toktype == tok_sym)
				{
					if (StrCharLast(rescrt->tok_content) != ';')
					{
						srs(rescrt->tok_content, StrHeapAppend(rescrt->tok_content, ";"));
						rescrt->toklen++;
					}
				}
				else
				{
					token* tmptok = TokNew(StrHeap(";"), rescrt->parent, 0, rescrt, 0, tok_sym, rescrt->nest);
					rescrt->right = tmptok;
					rescrt = tmptok;
				}
				continue;
			}
		}
		else
		{
			rescrt = rescrt->right = TokNew(StrHeapN(crt->addr, StrLength(crt->addr)), rescrt->parent, 0, rescrt, 0, crt->type, crtnest);
		}
		goto loop0;
	}
	StrTokenClear(tokfirst);
	if (rescrt->toktype != tok_sym && StrCompare(rescrt->tok_content, ";"))
	{
		token* compens = TokNew(StrHeap(";"), 0, 0, rescrt, 0, tok_sym, rescrt->nest);
		rescrt->right = compens;
	}
	if (resfirst->right)// Do not care the subitem of the NOP
	{
		resfirst = resfirst->right;
		if (resfirst->left->subfirst)
			TokScpClr(resfirst->left->subfirst);
		memfree(resfirst->left->tok_content);
		memfree(resfirst->left);
		resfirst->left = 0;
		return resfirst;
	}
	else
	{
		TokScpClr(resfirst);
		return 0;
	}
*/
}

/*
void static TokScpPrintSub(token* headtok, size_t nest, int a)
{
	// a=0 for element a=1 for express
	if (a) do
	{
		for (size_t i = 0; i < nest; i++)putchar('\t');
		printf("Tok[%s] {%s}", StrTokenPrintAll_GetTypeName(headtok->toktype), (headtok->toktype == tok_space) ? "" : headtok->tok_content);
		
		if (headtok->subfirst)
		{
			printf("{");
			TokScpPrintSub(headtok->subfirst, nest + 1, 0);
			printf("}");
		}
		
		if (headtok->toktype == tok_anyexp && headtok->sublast)
		{
			puts("");
			TokScpPrintSub(headtok->sublast, nest + 1, 1);
		}
		else putchar('\n');
	}
	while ((headtok = headtok->right));
	else do
	{
		printf(" %s ", (headtok->toktype == tok_space) ? "" : headtok->tok_content);
		if (headtok->subfirst)
		{
			printf("(");
			TokScpPrintSub(headtok->subfirst, nest + 1, 0);
			printf(")");
		}
	}
	while (headtok = headtok->right);
}

void TokScpPrint(token* headtok)
{
	TokScpPrintSub(headtok, 0, 1);
}

*/





