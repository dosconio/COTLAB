// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Task
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#ifndef _INC_CONTASK
#define _INC_CONTASK

#include <tnode>
#include <inode>
#include <nnode>

enum consrc_t { CONTASK_FILE, CONTASK_BUF };

enum stage_t { STAGE_RAW, STAGE_PREPED, STAGE_PARSED, STAGE_EXECUTED, STAGE_FAILED, STAGE_OUTSIDE };

class Contask {
private:
	consrc_t consrc;
	const char* filename;
	stage_t stage;
	uni::InodeChain* list_macros;// > nsens_objs
	uni::InodeChain* list_usenss;// > isens_objs
	uni::InodeChain* list_isenss;// inode::property as [...|MUTABLE]
	bool Link();
public:
	uni::TokenParseUnit* tpu;
	uni::NestedParseUnit* npu;
	uni::TnodeChain* returns;

	Contask(const char* fname = "cotemp.bat", consrc_t srctyp = CONTASK_FILE, uni::InodeChain** idens = 0);
	~Contask();
	void Prep();
	void Parse();
	void Execute();
	void All() {
		if (stage == STAGE_RAW) Prep();
		if (stage == STAGE_PREPED) Parse();
		if (stage == STAGE_PARSED) Execute();
	}
	void PrintDebug();

};


#endif // _INC_CONTASK
