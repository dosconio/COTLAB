// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: for linkage(built-in and outside) and execution
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include "cothead.h"
#include "contask.h"
#include "node"
#include "stdio.h"

static bool StrTokenNestLinkage(uni::Nnode* inp, uni::NodeChain* togc);


const char* builtin_iden[] = { 0
	//"PREPOSI","PRENEGA","ARIADD","ARISUB","ARIMUL","ARIDIV","ARIREM","ARIPOW","ARIFACT",
	//"_dbg_test", "system", "load", "int", "ASSIGN",
	//"sin", "cos", "tan", "asin","acos","atan","sinh", "cosh", "tanh", "asinh","acosh","atanh",

};// Catious strcat here

void* builtin_link[] = { 0
	//DtrPREPOSI, DtrPRENEGA, DtrARIADD, DtrARISUB, DtrARIMUL, DtrARIDIV, DtrARIREM, DtrARIPOW, //DtrARIFACT,
	//Dtr_dbg_test, Dtr_system, Dtr_load, Dtr_int,DtrASSIGN,
	//DtrSin, DtrCos, DtrTan,DtrASin,DtrACos,DtrATan,DtrSinh, DtrCosh, DtrTanh,DtrASinh,DtrACosh,DtrATanh,

};

void* CotCopy(void* inp, stduint typ) {
	if (typ == dt_float) {
		return StrHeap((char*)inp);
		//{}
	} 
	else {
		return StrHeap((char*)inp);
	}
}

bool Contask::Link() {
	// Check that each line only has one item;
	uni::Nnode* crtnes = this->npu->GetNetwork()->Root();
	if (crtnes && (crtnes = crtnes->next)) do if (crtnes->row == crtnes->left->row) {
		crtrow = crtnes->row, crtcol = crtnes->col; // -(crtnes->addr ? StrLength(crtnes->addr) : 0);
		return false;
	} while (crtnes = crtnes->next);

	// Link
	return StrTokenNestLinkage(this->npu->GetNetwork()->Root(), this->npu->TokenOperatorGroupChain);
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
			} while (crtnod = crtnod->next);
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
		} else if (crt->type == tok_number) {
			if (StrIndexChars(crt->addr, "ij"))
			{
				//crt->type = dt_num;
				//coe* tmpcoe = CoeFromLocale(crt->addr);
				//srs(crt->addr, NumNewComplex("+0", "+0", "+1", tmpcoe->coff, tmpcoe->expo, tmpcoe->divr));
				//CoeDel(tmpcoe);
			}
			else if (StrIndexCharsExcept(crt->addr, "0123456789"))
			{
				//srs(crt->offs, CoeFromLocale(crt->addr));
			}
			else // flat integer
			{
				//crt->type = dt_int;
				//srs(crt->addr, StrHeapAppend("+", crt->addr));
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
			///f_io->Onfree()
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

void OpARIADD(uni::DnodeChain* io) {
	Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;//{ERRO}
	Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		printf("inp: %s ans %s\n", first->offs, second->offs);
		// CoeAdd
		io->Remove(second);
	}
}
