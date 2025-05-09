﻿// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: for linkage(built-in and outside) and execution
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <stdio.h>
#include <cpp/nodes>
#include "../inc/cothead.h"
#include "../inc/contask.h"
extern pureptr_t glb;

#define FuncOp2(iden,sym) void Op##iden(uni::DnodeChain* io) {\
	uni::Dnode* first = io->Root();\
	if (!first->next || io->Count() != 2) return;\
	uni::Dnode* second = first->next;\
	if (first->type == tok_number && second->type == tok_number) {\
		uni::Coe& coe_des = *(uni::Coe*)first->offs;\
		uni::Coe& coe_src = *(uni::Coe*)second->offs;\
		coe_des sym coe_src;\
		io->Remove(second);\
	}\
}

FuncOp2(ARIPOW, ^=)
void OpARIMUL(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des *= coe_src;
		io->Remove(second);
	}
	//else if (first->type == tok_string && second->type == tok_number) {

	//}
}
FuncOp2(ARIDIV, /=)
void OpARIADD(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des += coe_src;
		io->Remove(second);
	}
	else if (first->type == tok_string && second->type == tok_string) {
		srs(first->offs, StrHeapAppend((char*)first->offs, (char*)second->offs));
		io->Remove(second);
	}
}
FuncOp2(ARISUB, -=)

#define FuncBasic(iden) void Fn##iden(uni::DnodeChain* io) {\
	uni::Dnode* first = io->Root();\
	if (!first || io->Count() != 1) {\
		puts("Function " #iden " requires one argument.");\
		return;\
	}\
	if (first->type == tok_number) {\
		uni::Coe& coe_des = *(uni::Coe*)first->offs;\
		coe_des = coe_des.iden();\
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

void OpPREPOSI(uni::DnodeChain* io) { }
void OpPRENEGA(uni::DnodeChain* io) {
	if (io->Root()->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)io->Root()->offs;
		coe_des = -coe_des;
	}
}


void OpJBELOW(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des = coe_des < coe_src;
		io->Remove(second);
	}
}
void OpJBEEQU(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des = coe_des <= coe_src;
		io->Remove(second);
	}
}
void OpJGREAT(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des = coe_des > coe_src;
		io->Remove(second);
	}
}
void OpJGREQU(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des = coe_des >= coe_src;
		io->Remove(second);
	}
}


void OpJEQUAL(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des = coe_des == coe_src;
		io->Remove(second);
	}
	else if (first->type == tok_string && second->type == tok_string) {//{} To Bool
		srs(first->offs, new uni::Coe(!StrCompare(first->addr, second->addr)));
		first->type = tok_number;
		io->Remove(second);
	}
}
void OpJNOTEQ(uni::DnodeChain* io) {
	uni::Dnode* first = io->Root();
	if (!first->next || io->Count() != 2) return;
	uni::Dnode* second = first->next;
	if (first->type == tok_number && second->type == tok_number) {
		uni::Coe& coe_des = *(uni::Coe*)first->offs;
		uni::Coe& coe_src = *(uni::Coe*)second->offs;
		coe_des = coe_des != coe_src;
		io->Remove(second);
	}
	else if (first->type == tok_string && second->type == tok_string) {//{} To Bool
		srs(first->offs, new uni::Coe(!!StrCompare(first->addr, second->addr)));
		first->type = tok_number;
		io->Remove(second);
	}
}




//

extern "C" void erro(char* msg) {
	//...
}

//{} into Contask::
const char* builtin_iden[] = { "sin", "cos", "tan", "asin","acos","atan","sinh", "cosh", "tanh", "asinh","acosh","atanh",
//"_dbg_test", "system", "load", "int", "ASSIGN",

};// Catious strcat here

typedef void (*builtin_func_t)(uni::DnodeChain* io);

builtin_func_t builtin_link[] = { FnSin, FnCos, FnTan, FnASin,FnACos,FnATan,FnSinh, FnCosh, FnTanh, FnASinh,FnACosh,FnATanh,
//Dtr_dbg_test, Dtr_system, Dtr_load, Dtr_int,DtrASSIGN,
};

//
static bool StrTokenNestLinkage(uni::Nnode* inp, uni::NodeChain* togc) {
	for (uni::Nnode* crt = inp; crt; crt = crt->next) {
		if (crt->subf && !StrTokenNestLinkage(crt->subf, togc))
			return false;
		if (crt->type == tok_func && !crt->GetMagnoField().bind && crt->addr) {
			for0(i, numsof(builtin_iden)) if (builtin_iden[i] && !StrCompare(builtin_iden[i], crt->addr)) {
				crt->GetMagnoField().bind = builtin_link[i];
				break;
			}
			if (!crt->GetMagnoField().bind) {
				crtrow = crt->GetMagnoField().row;
				crtcol = crt->GetMagnoField().col; // -(crt->addr ? StrLength(crt->addr) : 0);
				crtmsg = StrHeap(crt->addr);
				return false;
			}
		}
	}
	return true;
}

static void LinkNumber(uni::Nnode* inp, uni::NodeChain* togc) {
	for (uni::Nnode* crt = inp; crt; crt = crt->next) {
		if (crt->subf) LinkNumber(crt->subf, togc);
		if (crt->type == tok_number) {
			if (StrIndexChars(crt->addr, "ij"))
			{
				crt->type = tok_string;
				//crt->type = dt_num;
				//coe* tmpcoe = CoeFromLocale(crt->addr);
				//srs(crt->addr, NumNewComplex("+0", "+0", "+1", tmpcoe->coff, tmpcoe->expo, tmpcoe->divr));
				//CoeDel(tmpcoe);
			}
			else if (StrIndexCharsExcept(crt->addr, "0123456789") || true)
			{
				uni::Coe* tmpcoe = new uni::Coe(CoeFromLocale(crt->addr));
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
	if (crtnes && (crtnes = crtnes->next)) do if (crtnes->GetMagnoField().row == crtnes->left->GetMagnoField().row) {
		crtrow = crtnes->GetMagnoField().row, crtcol = crtnes->GetMagnoField().col; // -(crtnes->addr ? StrLength(crtnes->addr) : 0);
		state = false;
	} while (crtnes = crtnes->next);
	LinkNumber(this->npu->GetNetwork()->Root(), this->npu->TokenOperatorGroupChain);
	// Link
	if (!state)
	{
		this->npu->~NestedParseUnit();
		return state;
	}
	return StrTokenNestLinkage(this->npu->GetNetwork()->Root(), this->npu->TokenOperatorGroupChain) && state;
}

bool CotExecuate(uni::Nnode* inp, uni::NnodeChain* nc, uni::Nnode*& parencrt, IdenChain* list_sens) {
	IdenObject* inod;
	for (uni::Nnode* crt = inp; crt; crt && (crt = crt->next)) {
		if (crt->subf && !CotExecuate(crt->subf, nc, crt, list_sens)) return false;
		if (crt->type == tok_any) continue;
		if (crt->type == tok_identy && !crt->GetMagnoField().bind && !crt->subf && crt->addr) {
			if (list_sens && (inod = list_sens[0][crt->addr])) {
				srs(crt->offs, CotCopy(inod->offs, crt->type = inod->type));
			}
			else for0(i, numsof(builtin_iden)) {
				if (builtin_iden[i] && !StrCompare(builtin_iden[i], crt->addr))
					crt->GetMagnoField().bind = builtin_link[i];
			}
		}
		if (crt->type == tok_func && !crt->addr && crt->subf && !crt->subf->next) {
			crt->offs = CotCopy(crt->subf->addr, crt->subf->type);
			crt->type = crt->subf->type;
			nc->Remove(crt->subf);
		}
		if (crt->GetMagnoField().bind) {
			uni::DnodeChain* f_io = new uni::DnodeChain();
			f_io->func_free = ((_tofree_ft)(ReleaseTofreeCotlab<uni::Dnode>));
			union {
				uni::Dnode* dcrt;
				uni::Nnode* ncrt;
			};
			for (ncrt = crt->subf; ncrt; ncrt = ncrt->next)
				f_io->Append(CotCopy(ncrt->addr, ncrt->type), false)->type = ncrt->type;
			crt->GetMagnoField().bind(f_io);
			nc->Receive(crt, f_io);
			delete (pureptr_t)f_io;//mfree(f_io);
			crt = nc->Remove(ncrt = crt);
			if (parencrt == ncrt) parencrt = crt;
		}
	}

	return true;
}

