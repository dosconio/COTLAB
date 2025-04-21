// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio, @ArinaMgk
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include "../inc/cothead.h"
#include "../inc/contask.h"
#include <cpp/parse.hpp>
#include <stdio.h>

static char cotbuf[0x1000];
static union {
	FILE* fp;
	const char* sp;
};//{TODO} out of class

static int fgetnext(void) { return fgetc(fp); }
static int sgetnext(void) { return (*sp) ? *sp++ : EOF; }

//{TODO} make COTLAB into a dynamic class
stduint crtrow, crtcol;
char* crtmsg;

//{TODE} template<typename type0> void ReleaseTofreeCotlab(type0* n);

Contask::Contask(const char* fname, consrc_t srctyp, IdenChain* idens) {
	consrc = srctyp;
	filename = fname;
	stage = STAGE_OUTSIDE;
	if (idens) {
		list_macros = &idens[0];
		list_usenss = &idens[1];
		list_isenss = &idens[2];
	}
	if (consrc == CONTASK_FILE)
		pinn = new CotInn(fgetnext);
	else if (consrc == CONTASK_BUF)
		pinn = new CotInn(sgetnext);
	npu = 0;
	stage = STAGE_RAW;
}

Contask::~Contask() {
	if ((stage == STAGE_EXECUTED || stage == STAGE_PARSED || stage == STAGE_FAILED) || dc.Count())
	{
		npu->~NestedParseUnit();
		npu->TokenOperatorGroupChain->~Chain();
		mfree(npu->TokenOperatorGroupChain);
	}
	else {
		if (lp) {
			lp->~LinearParser();
			mfree(lp);
		}
		dc.~Dchain();
		mfree(pinn);
	}
	mfree(npu);
}

void Contask::Prep() {
	sp = filename;
	if (stage != STAGE_RAW) return;
	stage = STAGE_PREPED; // tpm->TokenParse() ? STAGE_PREPED : STAGE_FAILED;
	lp = new uni::LinearParser(*pinn);
	lp->GHT = false;
	lp->method_string_double_quote = true;
	lp->Parse(dc);
	//#ifdef _DEBUGX
	//	for (auto i = dc.Root(); i; i = i->next) {
	//		printf(">[%s]\t%s\n", tab_tokentype[i->type], i->addr);
	//	}
	//#endif
}

void Contask::Parse() {
	using namespace uni;
	if (stage == STAGE_PREPED) {
		LinearParser::RemoveComments(dc);
		LinearParser::StringConcatenation(dc);
		//{} directive process
		npu = new uni::NestedParseUnit(dc, CotInitOperators(), sizeof(uni::mag_node_t));
		dc.~TnodeChain();
		lp->~LinearParser();
		mfree(lp);
		mfree(pinn);
		//
		npu->GetNetwork()->func_free = NnodeHeapFreeSimple;
		stage = STAGE_PARSED;
		if (!npu->Parse()) {
			stage = STAGE_FAILED;
			crtcol = crtrow = 0; crtmsg = 0;
			cabort(filename, "Parse failed");
			return;
		}
		npu->GetNetwork()->func_free = ((_tofree_ft)(ReleaseTofreeCotlab<uni::Nnode>));
		if (!Link()) {
			stage = STAGE_FAILED;
			void NnodePrintPrelink(const uni::Nnode * nnod, unsigned nest);
			if (npu->GetNetwork()) NnodePrintPrelink(npu->GetNetwork()->Root(), 0);
			cabort(filename, "Linkage failed");
			return;
		}

#ifdef _DEBUGX
		PrintDebug(); puts("");
#endif
	}

}

bool Contask::Execute() {
	if (stage == STAGE_FAILED || stage != STAGE_PARSED || !npu) return false;
	stage = STAGE_EXECUTED;
	uni::Nnode* n = npu->GetNetwork()->Root();
	if (!CotExecuate(n, npu->GetNetwork(), n, list_usenss)) {
		cabort(filename, "Execute failed");
		return false;
	}
	return true;
}

// ---- ---- ---- ----

// Cotlab Abort : Cancel using farjump
void cabort(const char* fname, const char* str) {
	//{} do not use union fname
	if (!fname)
		plogerro("R%[u] C%[u]", crtrow, crtcol);
	else plogerro("F\"%s\" [R%[u] C%[u]]", fname, crtrow, crtcol);
	crtcol = 0; crtrow = 0;
	if (crtmsg) {
		plogerro("%s (%s)", str, crtmsg);
		mfree(crtmsg);
	}
	else plogerro(" %s", str);
}

