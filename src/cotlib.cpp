// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: for linkage(built-in and outside) and execution
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include "../inc/cothead.h"
#include "../inc/contask.h"
#include "node"
#include "stdio.h"
#include <coear>

static bool StrTokenNestLinkage(uni::Nnode* inp, uni::NodeChain* togc);

#define isaritype(x)(x==dt_int||x==dt_float||x==dt_cplx||x==dt_posi)

extern "C" void erro(char* msg) {
	//...
}

const char* builtin_iden[] = { "sin", "cos", "tan", "asin","acos","atan","sinh", "cosh", "tanh", "asinh","acosh","atanh",
	//"_dbg_test", "system", "load", "int", "ASSIGN",

};// Catious strcat here

void* builtin_link[] = { FnSin, FnCos, FnTan, FnASin,FnACos,FnATan,FnSinh, FnCosh, FnTanh, FnASinh,FnACosh,FnATanh,
	//Dtr_dbg_test, Dtr_system, Dtr_load, Dtr_int,DtrASSIGN,
};

void* CotCopy(void* inp, stduint typ) {
	if (!inp || !typ) return 0;
	if (typ == dt_float) {
		uni::Coe* tmpcoe = zalcof(uni::Coe);
		new (tmpcoe) uni::Coe(*(uni::Coe*)inp);
		return tmpcoe;
		// return new uni::Coe(*(uni::Coe*)inp);
	} 
	else {
		return StrHeap((char*)inp);
	}
}

static void LinkNumber(uni::Nnode* inp, uni::NodeChain* togc) {
	for (uni::Nnode* crt = inp; crt; crt = crt->next) {
		if (crt->subf) LinkNumber(crt->subf, togc);
		if (crt->type == tok_number) {
			if (StrIndexChars(crt->addr, "ij"))
			{
				//crt->type = dt_num;
				//coe* tmpcoe = CoeFromLocale(crt->addr);
				//srs(crt->addr, NumNewComplex("+0", "+0", "+1", tmpcoe->coff, tmpcoe->expo, tmpcoe->divr));
				//CoeDel(tmpcoe);
			}
			else if (StrIndexCharsExcept(crt->addr, "0123456789") || true)
			{
				uni::Coe* tmpcoe = zalcof(uni::Coe);
				new (tmpcoe) uni::Coe(CoeFromLocale(crt->addr));
				srs(crt->offs, tmpcoe);
			}
			else // flat integer
			{
				//crt->type = dt_int;
				//srs(crt->addr, StrHeapAppend("+", crt->addr));
			}
		}
	}
}

bool Contask::Link() {
	bool state = true;
	// Check that each line only has one item;
	uni::Nnode* crtnes = this->npu->GetNetwork()->Root();
	if (crtnes && (crtnes = crtnes->next)) do if (crtnes->row == crtnes->left->row) {
		crtrow = crtnes->row, crtcol = crtnes->col; // -(crtnes->addr ? StrLength(crtnes->addr) : 0);
		state = false;
	} while (crtnes = crtnes->next);
	LinkNumber(this->npu->GetNetwork()->Root(), this->npu->TokenOperatorGroupChain);
	// Link
	return StrTokenNestLinkage(this->npu->GetNetwork()->Root(), this->npu->TokenOperatorGroupChain) && state;
}

// Bind Coe, Linkage, with error handling
static bool StrTokenNestLinkage(uni::Nnode* inp, uni::NodeChain* togc)
{
	for (uni::Nnode* crt = inp; crt; crt = crt->next) {
		if (crt->subf && !StrTokenNestLinkage(crt->subf, togc))
			return false;
		if (crt->type == tok_func) { // assert(crt->addr)
			Node* crtnod;
			if (!StrCompareN(crt->addr, "OP@", 3)) if (crtnod = togc->Root()) do {
				uni::TokenOperatorGroup* tmpopg = (uni::TokenOperatorGroup*)crtnod->offs;
				uni::TokenOperator tmpop;
				for0(i, tmpopg->count) {
					if ((tmpop = tmpopg->operators[i]).ident && !StrCompare(tmpop.ident, crt->addr)) {
						crt->bind = tmpop.bindfn;
						break;
					}
				}
			} while (crtnod = crtnod->next); else;
			else for0 (i, numsof(builtin_iden)) if (builtin_iden[i] && !StrCompare(builtin_iden[i], crt->addr)) {
				crt->bind = builtin_link[i];
				break;
			}
			if (!crt->bind) {
				crtrow = crt->row;
				crtcol = crt->col; // -(crt->addr ? StrLength(crt->addr) : 0);
				crtmsg = StrHeap(crt->addr);
				return false;
			}
		}
	}
	return true;
}

//

bool CotExecuate(uni::Nnode* inp, uni::NnodeChain* nc, uni::Nnode*& parencrt) {
	for (uni::Nnode* crt = inp; crt; crt && (crt = crt->next)) {
		if (crt->subf && !CotExecuate(crt->subf, nc, crt)) return false;
		if (crt->type == tok_any) continue;
		if (crt->bind) {
			uni::DnodeChain* f_io = zalcof(uni::DnodeChain);
			new (f_io) uni::DnodeChain(true);
			f_io->Onfree((_tofree_ft)(ReleaseTofreeCotlab<Dnode>));
			union {
				Dnode* dcrt;
				uni::Nnode* ncrt;
			};
			for (ncrt = crt->subf; ncrt; ncrt = ncrt->next)
				f_io->Append(CotCopy(ncrt->addr, ncrt->type), ncrt->type);
			((uni::_tok_bindfunc_t)crt->bind)(f_io);
			nc->Receive(crt, f_io);
			mfree(f_io);
			crt = nc->Remove(ncrt = crt);
			if (parencrt == ncrt) parencrt = crt;
		}
	}
	return true;
}

//

#define FuncOp2(iden,sym) void Op##iden(uni::DnodeChain* io) {\
	Dnode* first = io->Root();\
	if (!first->next || io->Count() != 2) return;\
	Dnode* second = first->next;\
	if (first->type == tok_number && second->type == tok_number) {\
		uni::Coe& coe_des = *(uni::Coe*)first->offs;\
		uni::Coe& coe_src = *(uni::Coe*)second->offs;\
		coe_des sym coe_src;\
		io->Remove(second);\
	}\
}

FuncOp2(ARIPOW,^=)
FuncOp2(ARIMUL,*=)
FuncOp2(ARIDIV,/=)
FuncOp2(ARIADD,+=)
FuncOp2(ARISUB,-=)

#define FuncBasic(iden) void Fn##iden(uni::DnodeChain* io) {\
	Dnode* first = io->Root();\
	if (!first || io->Count() != 1) return;\
	if (first->type == tok_number) {\
		uni::Coe& coe_des = *(uni::Coe*)first->offs;\
		coe_des.Srs(coe_des.iden());\
	}\
}

FuncBasic(Sin)
FuncBasic(Cos)
FuncBasic(Tan)
FuncBasic(ASin)
FuncBasic(ACos)
FuncBasic(ATan)
FuncBasic(Sinh)
FuncBasic(Cosh)
FuncBasic(Tanh)
FuncBasic(ASinh)
FuncBasic(ACosh)
FuncBasic(ATanh)
