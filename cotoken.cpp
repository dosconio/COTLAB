// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <stdio.h>
#include "cothead.h"
#include "contask.h"

const char* tab_tokentype[] =
{
	"NON", "ANY", "COM", "DIR", "SYM", "SPA", "XXX", "IDN",
	"STR", "NUM", "EOF", "FUN",
	"EOF","complex","position","wides",
	"u8str", // ...
};

// ---- ---- ---- ---- Operator List ---- ---- ---- ----
uni::TokenOperator // Builtin Prefix "OP@"
	sufmem{ ".","SUFMEMB" },
\
	arfact{ "!","ARIFACT" },
	sufadd{ "++","SUFADD" },
	sufsub{ "--","SUFSUB" },
	// {cancelled} sufme2{ "->","SUFMEMB" },
	// "[\1]", "ARRSSC", "{\1}", "OBRACE" // <0> "a[b]c => ARRSSC(a,b)c"
\
	preadd{ "++","OP@PREADD" },
	presub{ "--","OP@@PRESUB" },
	preposi{ "+","OP@PREPOSI" },
	prenega{ "-","PRENEGA" },
	prelognot{ "!","OP@LOGNOT" },
	prebitnot{ "~","OP@BITNOT" },
	prememb{ "*","OP@PREMEMB" },
	preaddr{ "&","OP@PREADDR" },
	presizeof{ "sizeof","OP@SIZEOF" },
\
	aripow{ "^","OP@ARIPOW" },
\
	arimul{ "*","OP@ARIMUL" },
	aridiv{ "/","OP@ARIDIV" },
	arirem{ "%","OP@ARIREM" },
\
	ariadd{ "+","OP@ARIADD",OpARIADD },
	arisub{ "-","OP@ARISUB" },
\
	bitshl{ "<<","OP@BITSHL" },
	bitshr{ ">>","OP@BITSHR" },
\
	jbelow{ "<","OP@JBELOW" },
	jbeequ{ "<=","OP@JBEEQU" },
	jgreat{ ">","OP@JGREAT" },
	jgrequ{ ">=","OP@JGREQU" },
\
	jequal{ "==","OP@JEQUAL" },
	jnoteq{ "!=","OP@JNOTEQ" },
\
	bit_and{ "&","OP@BITAND" },
\
	bit_xor{ "^","OP@BITXOR" },
\
	bit_or{ "|","OP@BITWOR" },
\
	logand{ "&&","OP@LOGAND" },
\
	logior{ "||","OP@LOGIOR" },
\
	ternar{ "\2?\2:\2","OP@TERNAR" },
\
	assign{ "=","OP@ASSIGN" },
	agnsum{ "+=","OP@AGNSUM" },
	agndif{ "-=","OP@AGNDIF" },
	agnpro{ "*=","OP@AGNPRO" },
	agnquo{ "/=","OP@AGNQUO" },
	agnrem{ "%=","OP@AGNREM" },
	agnshl{ "<<+","OP@AGNSHL" },
	agnshr{ ">>=","OP@AGNSHR" },
	agnand{ "&=","OP@AGNAND" },
	agnxor{ "^=","OP@AGNXOR" },
	agnor{ "|=","OP@ASGNOR" }
\
	// blkjdg{ "\2if\2:\2","OP@BLKJDG" }
;

uni::TokenOperator \
opapd001[] = {sufmem},
opsuffix[] = {arfact, sufadd, sufsub},
opprefix[] = {preadd, presub, preposi, prenega, prelognot, prebitnot, prememb, preaddr, presizeof},
opmid001[] = {aripow},
opmid002[] = {arimul, aridiv, arirem},
opmid003[] = {ariadd, arisub},
opmid004[] = {bitshl, bitshr},
opmid005[] = {jbelow, jbeequ, jgreat, jgrequ},
opmid006[] = {jequal, jnoteq},
opmid007[] = {bit_and},
opmid008[] = {bit_xor},
opmid009[] = {bit_or},
opmid010[] = {logand},
opmid011[] = {logior},
//{} opmid012[] = {ternar},
opmid013[] = {assign, agnsum, agndif, agnpro, agnquo, agnrem, agnshl, agnshr, agnand, agnxor, agnor}
// opmid014[] = {blkjdg}
;

// ---- ---- ---- ---- Initialization ---- ---- ---- ----

#define togsym(a) a,numsof(a)

uni::NodeChain* CotInitOperators() {
	// single using limit until C++17
	using uni::NodeChain;
	using uni::TokenOperatorGroup;

	typedef TokenOperatorGroup TOG;
	TOG* grp_append_0 = new TOG(togsym(opapd001), true);
	TOG* grp00_suffix = new TOG(togsym(opsuffix), true, 1);
	TOG* grp01_prefix = new TOG(togsym(opprefix), false, 1);
	TOG* grpxx_mid001 = new TOG(togsym(opmid001));
	TOG* grpxx_mid002 = new TOG(togsym(opmid002));
	TOG* grpxx_mid003 = new TOG(togsym(opmid003));
	TOG* grpxx_mid004 = new TOG(togsym(opmid004));
	TOG* grpxx_mid005 = new TOG(togsym(opmid005));
	TOG* grpxx_mid006 = new TOG(togsym(opmid006));
	TOG* grpxx_mid007 = new TOG(togsym(opmid007));
	TOG* grpxx_mid008 = new TOG(togsym(opmid008));
	TOG* grpxx_mid009 = new TOG(togsym(opmid009));
	TOG* grpxx_mid010 = new TOG(togsym(opmid010));
	TOG* grpxx_mid011 = new TOG(togsym(opmid011));
	//{} TOG* grpxx_mid012 = new TOG(opmid012, numsof(opmid012), false, 3);
	TOG* grpxx_mid013 = new TOG(opmid013, numsof(opmid013), false);
	// TokenOpeartorGroup grpxx_mid014(opmid014, numsof(opmid014), false, 3);
	//
	NodeChain* nc = zalcof(NodeChain);
	new (nc) NodeChain(false);
	nc->Append(grp_append_0);
	nc->Append(grp00_suffix);
	nc->Append(grp01_prefix);
	nc->Append(grpxx_mid001);
	nc->Append(grpxx_mid002);
	nc->Append(grpxx_mid003);
	nc->Append(grpxx_mid004);
	nc->Append(grpxx_mid005);
	nc->Append(grpxx_mid006);
	nc->Append(grpxx_mid007);
	nc->Append(grpxx_mid008);
	nc->Append(grpxx_mid009);
	nc->Append(grpxx_mid010);
	nc->Append(grpxx_mid011);
	nc->Append(grpxx_mid013);
	//{TEMP} Complete in the near future ...
	return nc;
}

// ---- ---- ---- ---- Destructure ---- ---- ---- ----

// Destructure object according to type
void CotResourceRemove(void* obj, stduint typ)
{
	if (!obj) return;
	switch (typ)
	{
		/*case tok_number:
			CoeDel(obj);
			break;
		case dt_num:
			NumDel(obj);
			break;*/
	default:
		memf(obj);
		break;
	}
}




// ---- ---- ---- ---- Print ---- ---- ---- ----

static void printtok(char* addr, stduint typ, bool readonly = false) {
	if (typ >= numsof(tab_tokentype)) return;
	printf("[");
	if (readonly) printf("(readonly)");
	printf("%s] %s", tab_tokentype[typ], addr);
	puts("");
}

static void NnodePrint(const uni::Nnode* nnod, unsigned nest)
{
	const uni::Nnode* crt = nnod;
	while (crt)
	{
		for0(i, nest) printf(i + 1 == _LIMIT ? "->" : "--");
		printtok(crt->addr, crt->type);
		if (crt->subf) NnodePrint(crt->subf, nest + 1);
		crt = crt->next;
	}
}

void Contask::PrintDebug() {
	// linear token
	if (stage == STAGE_PREPED || stage == STAGE_PARSED || stage == STAGE_EXECUTED || (stage == STAGE_FAILED && npu && npu->GetNetwork()->Root())); else
		return;
	if (stage == STAGE_PREPED) {
		uni::Tnode* crttn = tpu->GetChain()->Root();
		if (!crttn->next) return;
		crttn = crttn->next;
		while (crttn) {
			printtok((char*)crttn->offs, crttn->type);
			crttn = crttn->next;
		}
	}
	else if (npu && npu->GetNetwork() && npu->GetNetwork()->Count()) 
		NnodePrint(npu->GetNetwork()->Root(), 0);
}

void InodePrint(uni::InodeChain* ic) {
	uni::Inode* crt = ic->Root();
	if (crt) do	printtok(crt->addr, crt->type, crt->readonly); while (crt = crt->next);
}

