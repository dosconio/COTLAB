// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#ifndef _INC_CONTASK
#define _INC_CONTASK

#include <cpp/nodes>
#include <cpp/parse.hpp>

enum consrc_t { CONTASK_FILE, CONTASK_BUF };

enum stage_t { STAGE_RAW, STAGE_PREPED, STAGE_PARSED, STAGE_EXECUTED, STAGE_FAILED, STAGE_OUTSIDE };

class CotInn : public uni::IstreamTrait {
public:
	int(*getnext)();
	CotInn(int(*_getnext)()) : getnext(_getnext) {

	}

	virtual int inn() {
		return getnext();
	}

};

class Contask {
private:
	consrc_t consrc;
	const char* filename;
	stage_t stage;
	IdenChain* list_macros;// > nsens_objs
	IdenChain* list_usenss;// > isens_objs
	IdenChain* list_isenss;// inode::property as [...|MUTABLE]
	bool Link();
public:
	//uni::TokenParseManager* tpm;
	uni::LinearParser* lp;
	uni::Dchain dc;
	CotInn* pinn;
	//
	uni::NestedParseUnit* npu;
	uni::TnodeChain* returns;

	Contask(const char* fname = "cotemp.bat", consrc_t srctyp = CONTASK_FILE, IdenChain* idens = 0);
	~Contask();
	void Prep();
	void Parse();
	bool Execute();
	void All() {
		if (stage == STAGE_RAW) Prep();
		if (stage == STAGE_PREPED) Parse();
		if (stage == STAGE_PARSED) Execute();
	}
	void PrintDebug();
	uni::Tnode* tpmRoot() { return (uni::Tnode*)dc.Root(); }
};

struct cotnode {
	// same with tnode
	stduint row;
	stduint col;
	//
	uni::_tok_bindfunc_t bind;
};

inline cotnode& refCnode(uni::Nnode* nod) {
	return *(cotnode*)(nod->GetExtnField());
}

#endif // _INC_CONTASK
