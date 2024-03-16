// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <cinc>
#include <alice.h>
#include <aldbg.h>
#include <cinc>
#include <new>
#include <tnode>
#include <nnode>
#include <stdio.h>
#include "cothead.h"
#include "contask.h"

static const char* TnodeTypename[]{
	"NON", "ANY", "COM", "DIR", "SYM", "SPA", "XXX",
	"IDN", "STR", "NUM"
};

const char* tab_tokentype[] =
{
	"none", "any", "comment", "directive",
	"symbol", "space", "unknown", "identifier",
	"string", "number", "EOF","function",
	"","","","",
	"func",
	"int","~float~",
	"complex","posi",
	"~str~","astr","u8str",
};

// ---- ---- ---- ---- Operator List ---- ---- ---- ----
const char\
	* OperatorSuff[] = { "!","ARIFACT","++","SUFADD","--","SUFSUB",".","SUFMEMB","->","SUFMEMB",
	"[\1]","ARRSSC","{\1}", "OBRACE" },// <0> "a[b]c => ARRSSC(a,b)c"
	* OperatorPref[] = { "++","PREADD","--","PRESUB","+","PREPOSI","-","PRENEGA",
		"!","LOGNOT","~","BITNOT","*","PREMEMB","&","PREADDR", "\0sizeof", "SIZEOF" },// <1>
	* OperatorLR0[] = { "^", "ARIPOW" },// <1.5>
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

#define _OPERATOR_GROUP_LEVEL_MAX 4// <GOAL> 14

// ---- ---- ---- ---- Print ---- ---- ---- ----

static void NnodePrint(const uni::Nnode* nnod, unsigned nest)
{
	const uni::Nnode* crt = nnod;
	while (crt)
	{
		for0(i, nest) putchar('\t');
		//if (crt->type == dt_float)
		//	printf("Nnode [%s] %lf\n", tokentype_iden[crt->type], CoeToDouble((void*)crt->addr));
		//else if (crt->type == dt_num)
		//{
		//	printf("Nnode [%s] [%lf %lfj]\n", tokentype_iden[crt->type], CoeToDouble(&((numa*)crt->addr)->Real), CoeToDouble(&((numa*)crt->addr)->Imag));
		//}
		//else
		printf("[%s] %s\n", (char*)tab_tokentype[crt->type], (char*)crt->offs);
		if (crt->subf) NnodePrint(crt->subf, nest + 1);
		crt = crt->next;
	}
}

void Contask::PrintDebug() {
	// linear token
	if (stage == STAGE_PREPED || stage == STAGE_PARSED || stage == STAGE_EXECUTED); else
		return;
	if (stage == STAGE_PREPED) {
		uni::Tnode* crttn = tpu->GetChain()->Root();
		if (!crttn->next) return;
		crttn = crttn->next;
		while (crttn) {
			printf("[%s] %s\n", TnodeTypename[(byte)crttn->type], (char*)crttn->offs);
			crttn = crttn->next;
		}
	}
	else if (npu && npu->GetNetwork() && npu->GetNetwork()->Count()) 
		NnodePrint(npu->GetNetwork()->Root(), 0);
}
