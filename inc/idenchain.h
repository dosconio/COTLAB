#ifndef __COTIDENS
#define __COTIDENS

#include <c/mnode.h>
#include "cothead.h"

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

	void Modify(const char* iden, pureptr_t offs, datatype typ,  bool mutabl) {
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


#endif
