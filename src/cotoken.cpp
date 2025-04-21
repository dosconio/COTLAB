// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <stdio.h>
#include "../inc/cothead.h"
#include "../inc/contask.h"

const char* tab_tokentype[] =
{
	"NON", "ANY",
	"SYM", "SPA", "NUM", "IDN", "XXX",
	"COM", "DIR", "STR",  "EOF", "FUN",
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
preposi{ "+","OP@PREPOSI",OpPREPOSI },
prenega{ "-","OP@PRENEGA",OpPRENEGA },
prelognot{ "!","OP@LOGNOT" },
prebitnot{ "~","OP@BITNOT" },
prememb{ "*","OP@PREMEMB" },
preaddr{ "&","OP@PREADDR" },
presizeof{ "sizeof","OP@SIZEOF" },
\
aripow{ "^","OP@ARIPOW",OpARIPOW },
\
arimul{ "*","OP@ARIMUL",OpARIMUL },
aridiv{ "/","OP@ARIDIV",OpARIDIV },
arirem{ "%","OP@ARIREM" },
\
ariadd{ "+","OP@ARIADD",OpARIADD },
arisub{ "-","OP@ARISUB",OpARISUB },
\
bitshl{ "<<","OP@BITSHL" },
bitshr{ ">>","OP@BITSHR" },
\
jbelow{ "<" ,"OP@JBELOW",OpJBELOW },
jbeequ{ "<=","OP@JBEEQU",OpJBEEQU },
jgreat{ ">" ,"OP@JGREAT",OpJGREAT },
jgrequ{ ">=","OP@JGREQU",OpJGREQU },
\
jequal{ "==","OP@JEQUAL",OpJEQUAL },
jnoteq{ "~=","OP@JNOTEQ",OpJNOTEQ },// not != for "! is for factorial"
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
opapd001[] = { sufmem },
opsuffix[] = { arfact, sufadd, sufsub },
opprefix[] = { preadd, presub, preposi, prenega, prelognot, prebitnot, prememb, preaddr, presizeof },
opmid001[] = { aripow },
opmid002[] = { arimul, aridiv, arirem },
opmid003[] = { ariadd, arisub },
opmid004[] = { bitshl, bitshr },
opmid005[] = { jbelow, jbeequ, jgreat, jgrequ },
opmid006[] = { jequal, jnoteq },
opmid007[] = { bit_and },
opmid008[] = { bit_xor },
opmid009[] = { bit_or },
opmid010[] = { logand },
opmid011[] = { logior },
//{} opmid012[] = {ternar},
opmid013[] = { assign, agnsum, agndif, agnpro, agnquo, agnrem, agnshl, agnshr, agnand, agnxor, agnor }
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
	NodeChain* nc = new NodeChain();
	*nc
		<< (*grp_append_0)
		<< (*grp00_suffix)
		<< (*grp01_prefix)
		<< (*grpxx_mid001)
		<< (*grpxx_mid002)
		<< (*grpxx_mid003)
		<< (*grpxx_mid004)
		<< (*grpxx_mid005)
		<< (*grpxx_mid006)
		<< (*grpxx_mid007)
		<< (*grpxx_mid008)
		<< (*grpxx_mid009)
		<< (*grpxx_mid010)
		<< (*grpxx_mid011)
		<< (*grpxx_mid013);
	nc->func_free = uni::NodeHeapFreeSimple;
	return nc;
}


// ---- ---- ---- ---- Conversion ---- ---- ---- ----
//void ArithExplicitCoversionRise(dnode* const a, size_t typ)
//{
//	if (!isaritype(a->type) || !a->addr) return;
//	if (a->type == typ) return;
//	if (a->type == dt_num);
//	else if (a->type == dt_float)
//	{
//		if (typ == dt_num)
//		{
//			coe* tmpcoe = (void*)a->addr;
//			a->addr = (void*)NumNewComplex(tmpcoe->coff, tmpcoe->expo, tmpcoe->divr, "+0", "+0", "+1");
//			CoeDel(tmpcoe);
//		}
//		else if (typ == dt_int);
//		a->type = typ;
//	}
//	else if (a->type == dt_int)
//	{
//		if (typ == dt_num)
//		{
//			srs(a->addr, NumNewComplex(a->addr, "+0", "+1", "+0", "+0", "+1"));
//		}
//		else if (typ == dt_float)
//		{
//			srs(a->addr, CoeNew(a->addr, "+0", "+1"));
//		}
//		a->type = typ;
//	}
//}

//void ArithImplicitCoversion(dnode* const a, dnode* const b)
//{
//	if (!isaritype(a->type) || !isaritype(b->type)) return;
//	if (a->type == b->type)return;
//	if (a->type == dt_num || b->type == dt_num)
//	{
//		dnode* dest = a->type == dt_num ? b : a;
//		if (dest->type == dt_int)
//		{
//			srs(dest->addr, NumNewComplex(dest->addr, "+0", "+1", "+0", "+0", "+1"));
//		}
//		else if (dest->type == dt_float)
//		{
//			coe* tmpcoe = (coe*)dest->addr;
//			dest->addr = (void*)NumNewComplex(tmpcoe->coff, tmpcoe->expo, tmpcoe->divr, "+0", "+0", "+1");
//			CoeDel(tmpcoe);
//		}
//		dest->type = dt_num;
//	}
//	else if (a->type == dt_float || b->type == dt_float)
//	{
//		dnode* dest = a->type == dt_float ? b : a;
//		if (dest->type == dt_int)
//		{
//			srs(dest->addr, CoeNew(dest->addr, "+0", "+1"));
//		}
//		dest->type = dt_float;
//	}
//}
// 


void* CotCopy(void* inp, stduint typ) {
	if (!inp || !typ) return 0;
	if (typ == dt_float) {
		return new uni::Coe(*(uni::Coe*)inp);
	}
	else {
		return StrHeap((char*)inp);
	}
}

// ---- ---- ---- ---- Destructure ---- ---- ---- ----

// Destructure object according to type
// Do not free p_iobj
void CotResourceRemove(IdenObject* p_iobj)
{
	void*& obj = p_iobj->offs;
	if (!obj) return;
	switch (p_iobj->type) {
	case tok_number:
		((uni::Coe*)obj)->~Coe();
		break;
	default:
		break;
	}
	memf(obj);
}

void CotReleaseInode(pureptr_t offs) {
	Letvar(obj, uni::Dnode*, offs);
	mfree(obj->addr);// the identifier text in heap
	CotResourceRemove((IdenObject*)obj->type);// content data in heap
	mfree(obj->type);// the identifier text in heap
}

// ---- ---- ---- ---- Iden Chain ---- ---- ---- ----


IdenChain::IdenChain() : chn() {
	chn.func_comp = (_tocomp_ft)StrCompare;
	chn.refChain().func_free = CotReleaseInode;
}

IdenChain::~IdenChain() {

}

// ---- ---- ---- ---- Print ---- ---- ---- ----

static void printtok(pureptr_t addr, stduint typ, bool readonly = false) {
	if (typ >= numsof(tab_tokentype)) return;
	printf("[");
	if (readonly) printf("(readonly)");
	printf("%s] ", tab_tokentype[typ]);
	switch (typ)
	{
	case tok_number:
		printf("%lf = ", double(*(uni::Coe*)addr));
		printf("%s", uni::String(*(uni::Coe*)addr).reference());
		break;
	default:
		printf("%s", addr);
		break;
	}
	puts("");
}

static void NnodePrint(const uni::Nnode* nnod, unsigned nest)
{
	uni::Nnode* crt = (uni::Nnode*)nnod;
	while (crt)
	{
		for0(i, nest) printf(i + 1 == _LIMIT ? "->" : "--");
		//printf("%d %d:", refCnode(crt).row, refCnode(crt).col);
		printtok(crt->offs, crt->type);
		if (crt->subf) NnodePrint(crt->subf, nest + 1);
		crt = crt->next;
	}
}

void NnodePrintPrelink(const uni::Nnode* nnod, unsigned nest)
{
	uni::Nnode* crt = (uni::Nnode*)nnod;
	while (crt)
	{
		for0(i, nest) printf(i + 1 == _LIMIT ? "->" : "--");
		printf("%u %u:", crt->GetMagnoField().row, crt->GetMagnoField().col);;
		printf("[%s] ", tab_tokentype[crt->type]);
		if (crt->type != tok_number) printf("%s", crt->addr);
		puts("");
		if (crt->subf) NnodePrintPrelink(crt->subf, nest + 1);
		crt = crt->next;
	}
}

void Contask::PrintDebug() {
	// linear token
	if (stage == STAGE_FAILED) return;
	if (stage == STAGE_PREPED || stage == STAGE_PARSED || stage == STAGE_EXECUTED || (stage == STAGE_FAILED && npu && npu->GetNetwork()->Root())); else
		return;
	if (stage == STAGE_PREPED) {
		uni::Tnode* crttn = tpmRoot();
		if (!crttn->next) return;
		crttn = crttn->next;
		while (crttn) {
			printtok(crttn->offs, crttn->type);
			crttn = crttn->next;
		}
	}
	else if (npu && npu->GetNetwork())
		NnodePrint(npu->GetNetwork()->Root(), 0);
}

void InodePrint(IdenChain* ic) {
	uni::Dnode* crt = ic->refChain().Root();
	if (crt) do {
		IdenObject& io = *(IdenObject*)crt->type;
		printf("%s ", crt->addr);
		printtok(io.offs, io.type, !io.mutabl);
	} while (crt = crt->next);
}


