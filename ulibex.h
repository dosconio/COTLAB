// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
#ifndef _LIB_COMPLEMENT
#define _LIB_COMPLEMENT

#define _LIB_STRING_HEAP
#include <ustring.h>

static const char* datatype_iden[] =
{
	"func",
	"int","float",
	"num","posi",
	"str","astr","u8str",
};// {TODO} temporarily

enum datatype// for Nesnode.iden
{
	dt_any = 16,
	dt_func,

	dt_int,// ChrAr ptrdiff_t
	dt_float,// CdeAr double

	dt_num,// NumAr _Complex
	dt_posi,// NumAr 4D-vector

	dt_str,// ustring
	dt_astr,// wstring
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
};


// ---- ---- ---- ---- nodex.c ---- ---- ---- ----

void NnodeFree(void* n);
void DnodesReleaseCotlab(dnode* inp);
void CotRelease(tnode* inp);

// keep input `nest` zero.
void NnodePrint(const Nesnode* nnod, unsigned nest);

// Return the case. Do the check before calling this
int NnodeSymbolsDivide(nnode* inp, size_t width, size_t idx, nnode* parent);

// Rewrite the nnode
dnode* NnodeToDnode(nnode* inp);
tnode* NnodeToTnode(nnode* inp);


#endif
