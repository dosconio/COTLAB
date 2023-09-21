// ASCII GPL3 COTLAB Copyright (C) 2023 Dosconio
/* UNISYM Provided
- ChrAr -> CoeAr -> Arn/NumAr
- RsgAr -> RegAr -> HrnAr (Rsg aka Bcd)

- TenAr
*/

// The code is also its document;
// Any operator will be equivalent to its function form;
/* Component
- Pars & Exec & Shell
- Unisym: RegAr
- Erro Mech
- Pre-unisym: TenAr
*/
/* StrTokenParse
- Comment and spaces;
- Strcat;
- Discard any directive temporarily;
- Make the imm-value live
- - - - flag for marking existence of symbols of the scope, - - - -
- - - - excluding the sub-scopes.                           - - - -
- {WISH} Restructure for nested
- Convert all operators into function calling form;
- Check that each line only has one item;
- Echo for debug;
- Linkage; [StrTokenNestVariable: constant and variable; StrTokenLinkage: fill linked_func]
*/

#ifndef HrnCotlabParser
#define HrnCotlabParser

#include "ulibex.h"

typedef dnode* (*fstruc_t)(dnode* const);

// Return the case. Do the check before calling this
int NnodeSymbolsDivide(nnode* inp, size_t width, size_t idx, nnode* parent);

//
nnode* StrTokenParse(Tode* inp);

#endif