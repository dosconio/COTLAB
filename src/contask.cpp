// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <stdio.h>
#include "../inc/cothead.h"
#include "../inc/contask.h"

static char cotbuf[0x1000];
static union {
	FILE* fp;
	const char* sp;
};//{TODO} out of class

static int fgetnext(void) { return fgetc(fp); }
static void fseekp(ptrdiff_t l) { fseek(fp, (long)l, SEEK_CUR); }
static int sgetnext(void) { return (*sp) ? *sp++ : EOF; }
static void sseekp(ptrdiff_t l) { sp += l; }

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
		tpm = new uni::TokenParseManager(fgetnext, fseekp, cotbuf);
	else if (consrc == CONTASK_BUF)
		tpm = new uni::TokenParseManager(sgetnext, sseekp, cotbuf);
	tpm->dc.func_free = uni::DnodeHeapFreeSimple;
	tpm->dc.setExtnField(sizeof(uni::TnodeField));
	npu = 0;
	stage = STAGE_RAW;
}

Contask::~Contask() {
	if ((stage == STAGE_EXECUTED || stage == STAGE_PARSED) || !tpm)
		npu->~NestedParseUnit();
	else {
		tpm->~TokenParseManager();
		mfree(tpm);
	}
	mfree(npu);
}

void Contask::Prep() {
	sp = filename;
	if (stage != STAGE_RAW) return;
	stage = tpm->TokenParse() ? STAGE_PREPED : STAGE_FAILED;
}

void Contask::Parse() {
	if (stage == STAGE_PREPED) {
		npu = new uni::NestedParseUnit(tpm->dc, CotInitOperators(), sizeof(cotnode));
		mfree(tpm);
		npu->GetNetwork()->extn_field = sizeof(cotnode);
		npu->GetNetwork()->func_free = ((_tofree_ft)(ReleaseTofreeCotlab<uni::Nnode>));
		npu->Parse();
		if (!npu->parsed) {
			stage = STAGE_FAILED;
			crtcol = crtrow = 0; crtmsg = 0;
			cabort(filename, "Parse failed");
			return;
		}
		if (!Link()) {
			stage = STAGE_FAILED;
			cabort(filename, "Linkage failed");
			return;
		}
		stage = STAGE_PARSED;
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
	//{} ConStyleAbnormal();
	if (!fname)
		fprintf(stderr, "Error R%" PRIuPTR " C%" PRIuPTR, crtrow, crtcol);
	else fprintf(stderr, "Error F\"%s\" [R%" PRIuPTR " C%" PRIuPTR "]", fname, crtrow, crtcol);
	//{} ConStyleNormal();
	fprintf(stderr, " %s", str);
	crtcol = 0; crtrow = 0;
	if (crtmsg) {
		fprintf(stderr, " (%s)", crtmsg);
		memf(crtmsg);
	}
	fprintf(stderr, "\n");
}

