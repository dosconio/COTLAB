// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
#ifndef _INC_ARITHMETIC_SHELL
#define _INC_ARITHMETIC_SHELL
#define _SILVER_GARDEN_ARITHMETIC_Generation 3

#pragma warning(disable:6384)
#pragma warning(disable:6011)// nullptr check
#pragma warning(disable:4005)// redefine of macro

#define CotTitle "COTLAB Console [Datura Shell] Build " __DATE__ "\n"

#define _LIB_STRING_HEAP
#include <ustring.h>
#include <nodes.h>

extern inode** inods;
extern size_t LIB_CDE_PRECISE_SHOW;



enum datatype// for Nesnode.iden, all entity
{
	//dt_any,
	dt_func = 0x10,

	dt_int,// ChrAr ptrdiff_t
	dt_float = tok_number,// CdeAr double

	dt_num = dt_int + 2,// NumAr _Complex
	dt_posi,// NumAr 4D-vector

	dt_str = tok_string,// ustring
	dt_astr = dt_posi + 2,// wstring
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


// ---- ---- ---- ---- erro.c ---- ---- ---- ----

void cabort(const char* str, size_t row, size_t col, char* txt);

void NnodeReleaseTofreeCotlab(void* n);
void InodeReleaseTofreeElementCotlab(void* n);
void InodeReleaseTofreeCotlab(void* n);
void DnodesReleaseCotlab(dnode* inp);
void TnodesReleaseTofreeCotlab(void* inp);

// ---- ---- ---- ---- executor.c ---- ---- ---- ----

extern int COT_EXE_AUTOLF;

//
void InodePrint(inode*** inp);

// keep input `nest` zero.
void NnodePrint(const nnode* nnod, unsigned nest);

#endif
