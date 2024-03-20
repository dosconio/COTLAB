
#ifndef __COTHEAD
#define __COTHEAD

#include <cinc>
#include <alice.h>
#include <aldbg.h>
#include <cinc>
#include <new>
#include <tnode>
#include <inode>
#include <nnode>

extern const char* tab_tokentype[];

enum datatype// for Nesnode.iden, all entity
{
	dt_str = tok_string,// ustring
	dt_float = tok_number,// CdeAr double
	dt_int = tok__continue,// ChrAr ptrdiff_t
	dt_num = dt_int,// NumAr _Complex
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

template<typename type0> void ReleaseTofreeCotlab(type0* n)
{
	extern void CotResourceRemove(void* obj, stduint typ);
	CotResourceRemove((void*)n->offs, n->type);
	memf(n);
}

// cotoken.cpp
extern uni::NodeChain* CotInitOperators();
extern void InodePrint(uni::InodeChain* ic);

// cotlib.cpp
extern stduint crtrow, crtcol;
extern char* crtmsg;
void* CotCopy(void* inp, stduint typ);
bool CotExecuate(uni::Nnode* inp, uni::NnodeChain* nc, uni::Nnode*& parencrt);
void OpARIADD(uni::DnodeChain* io);

// contask.cpp
extern void cabort(const char* fname, const char* str);

#endif
