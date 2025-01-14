
#ifndef __COTHEAD
#define __COTHEAD

extern "C" int abs(int);

#include <cpp/basic>
#include <cpp/nodes>
#include <c/mnode.h>
#include <c/consio.h>

extern const char* tab_tokentype[];

enum datatype// for Nesnode.iden, all entity
{
	dt_str = tok_string,// ustring
	dt_float = tok_number,// CdeAr double
	dt_int = tok__continue,// ChrAr ptrdiff_t
	dt_cplx,// NumAr _Complex
	dt_posi,// NumAr 4D-vector

	dt_wstr,// wstring
	dt_u8str,// u8string

	// TenAr:
	dt_vector,// V[]
	dt_readonly,// Ro[] Tuple in Python
	dt_comb,// [] List in Python. (type of combination can be different)
	dt_ary,// dt_x[]
	dtx_dictionary,// from Python
	dt_tensor,// T[] ten* > vector
	dt_matrix,// M[], will by MtrAr

	dt_rangei,// [ ~ ] (by HrnAr x and t, t as summit)
	dt_rangec,// 0( , ) 3[ , ] 2[ , ) 1( , ] (by HrnAr x, y and t, t as type)

	dt_bool
};


struct IdenObject {
	enum datatype type;
	pureptr_t offs;
	bool mutabl;
};

struct IdenChain {
	uni::Mchain chn;
	IdenChain();
	~IdenChain();
	uni::Dchain& refChain() { return chn.refChain(); }

	IdenObject* operator[] (const char* iden) {
		uni::Dnode* res = nullptr;
		(void)chn.isExist((pureptr_t)iden, &res);
		return res ? (IdenObject*)res->type : nullptr;
	}

	void Modify(const char* iden, pureptr_t offs, datatype typ, bool mutabl) {
		IdenObject* io = zalcof(IdenObject), * tmp;
		io->type = typ;
		io->offs = offs;
		io->mutabl = mutabl;
		if (tmp = self[iden]) {
			if (!tmp->mutabl) return;
		}
		chn.Map((pureptr_t)StrHeap(iden), (pureptr_t)io);
	}

	void Remove(const char* iden, bool consider_mutabl = true) {
		uni::Dnode* dn = refChain().LocateNode((pureptr_t)iden, chn.func_comp);
		if (!dn) return;
		IdenObject* io = (IdenObject*)dn->type;
		if (consider_mutabl && !io->mutabl) return;
		refChain().Remove(dn);
	}

};

#define isaritype(x)(x==dt_int||x==dt_float||x==dt_cplx||x==dt_posi)

// cotoken.cpp
extern uni::NodeChain* CotInitOperators();
extern void InodePrint(IdenChain* ic);
void CotResourceRemove(IdenObject* p_iobj);

template<typename type0> void ReleaseTofreeCotlab(type0* n)
{
	IdenObject io = { (datatype)n->type, n->offs };
	CotResourceRemove(&io);
}
void CotReleaseInode(pureptr_t offs);

// cotlib.cpp
extern stduint crtrow, crtcol;
extern char* crtmsg;
void* CotCopy(void* inp, stduint typ);
bool CotExecuate(uni::Nnode* inp, uni::NnodeChain* nc, uni::Nnode*& parencrt, IdenChain* list_sens);
void OpARIPOW(uni::DnodeChain* io);
void OpARIMUL(uni::DnodeChain* io);
void OpARIDIV(uni::DnodeChain* io);
void OpARIADD(uni::DnodeChain* io);
void OpARISUB(uni::DnodeChain* io);
void FnSin(uni::DnodeChain* io);
void FnCos(uni::DnodeChain* io);
void FnTan(uni::DnodeChain* io);
void FnASin(uni::DnodeChain* io);
void FnACos(uni::DnodeChain* io);
void FnATan(uni::DnodeChain* io);
void FnSinh(uni::DnodeChain* io);
void FnCosh(uni::DnodeChain* io);
void FnTanh(uni::DnodeChain* io);
void FnASinh(uni::DnodeChain* io);
void FnACosh(uni::DnodeChain* io);
void FnATanh(uni::DnodeChain* io);

// contask.cpp
extern void cabort(const char* fname, const char* str);

#endif
