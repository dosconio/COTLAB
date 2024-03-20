// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
#ifndef _INC_ARITHMETIC_SHELL
#define _INC_ARITHMETIC_SHELL
#define _SILVER_GARDEN_ARITHMETIC_Generation 3

#pragma warning(disable:6384)
#pragma warning(disable:6011)// nullptr check
#pragma warning(disable:4005)// redefine of macro



#define _LIB_STRING_HEAP
#include <ustring.h>
#include <nodes.h>

extern inode** inods;
extern size_t LIB_CDE_PRECISE_SHOW;






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
