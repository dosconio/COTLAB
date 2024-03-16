// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <cinc>
#include <alice.h>
#include <aldbg.h>
#include <cinc>
#include "contask.h"
#include <stdio.h>
#include <fstream>
#include <new>
#include <nnode>

static char cotbuf[0x1000];
static union {
	FILE* fp;
	const char* sp;
};//{TODO} out of class


static int fgetnext(void) { return fgetc(fp); }
static void fseekp(ptrdiff_t l) { fseek(fp, (long)l, SEEK_CUR); }
static int sgetnext(void) { return (*sp) ? *sp++ : EOF; }
static void sseekp(ptrdiff_t l) { sp += l; }

void NnodeReleaseTofreeCotlab(void* n);

Contask::Contask(const char* fname, consrc_t srctyp, uni::InodeChain** idens) {
	//{TODO} receive varlist inode[3]*
	consrc = srctyp;
	filename = fname;
	stage = STAGE_OUTSIDE;
	tpu = zalcof(uni::TokenParseUnit);
	if (idens) {
		list_macros = idens[0];
		list_usenss = idens[1];
		list_isenss = idens[2];
	}
	if (consrc == CONTASK_FILE)
		new (tpu)uni::TokenParseUnit(fgetnext, fseekp, cotbuf);
	else if (consrc == CONTASK_BUF)
		new (tpu)uni::TokenParseUnit(sgetnext, sseekp, cotbuf);
	npu = 0;
	stage = STAGE_RAW;
}

Contask::~Contask() {
	if (stage == STAGE_EXECUTED || stage == STAGE_PARSED)
		npu->~NestedParseUnit();
	else tpu->~TokenParseUnit();
	mfree(tpu);
	mfree(npu);
}

void Contask::Prep() {
	sp = filename;
	if (stage != STAGE_RAW) return;
	stage = tpu->TokenParse() ? STAGE_PREPED : STAGE_FAILED;
}

void Contask::Parse() {
	sp = filename;
	if (stage == STAGE_PREPED) {
		npu = zalcof(uni::NestedParseUnit);
		new (npu) uni::NestedParseUnit(*tpu->GetChain());
		npu->GetNetwork()->Onfree(NnodeReleaseTofreeCotlab);
		npu->Parse();
		if (!npu->parsed) {
			stage = STAGE_FAILED;
			return;
		}
		stage = STAGE_PARSED;
	}
	
}

void Contask::Execute() {
	if (stage != STAGE_PARSED) return;
	stage = STAGE_EXECUTED;
	//...
}

//

// Destructure object according to type
inline static void CotResourceRemove(void* obj, size_t typ)
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

void NnodeReleaseTofreeCotlab(void* n)
{
	auto nod = (uni::Nnode*)n;
	CotResourceRemove((void*)nod->offs, nod->type);
	memf(n);
}





