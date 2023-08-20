// ASCII
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
/*
- Comment and spaces;
- Strcat;
- Discard any directive temporarily;
- Convert all operators into function calling form;
- Check that each line only has either one IEV or one NUM/STR/IDEN, or it will be bad;
- Echo for debug (for IEV);
- Convert each IEV to function-calling-form and make linkage;
- {USELESS} Make EXEC chain;
- {TODO} Check no more symbols except parenx;
- {TODO} Check parens and parend matched;
*/

#ifndef HrnCotlabParser
#define HrnCotlabParser

#include <ustring.h>

enum datatype
{
	dt_func,

	// RegAr can operate:
	dt_uint,
	dt_sint,// > uint
	dt_float,// > sint

	// HrnAr:
	dt_num,// >float (default _Complex)
	dt_posi,// 4D vector
	
	dt_str,// ASCII string, by ustring

	// {wstring}
	dt_astr,// ANSI string, by wchar
	
	dt_u8str,//{TODO} UTF-8 string, by nothing

	// TenAr&Op
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

typedef struct
{
	enum datatype ptype;
	void* content;
} fpara, expr;

typedef struct fstruc
{
	char* iden;
	fpara* paras;// constant
	expr* (*flink)(struct fstruc* const fparas);// func_link
	unsigned int counts;//{TODO} limit
} fstruc;

typedef Dnode token;

typedef struct InterElseValue
{
	char* fname;
	Tode* subfirst;
} iev;// can be into `fstruc` with the enough information.
// difference: iev for toktype, fstruc for datatype

void StrTokenParse(Tode* inp);


#endif