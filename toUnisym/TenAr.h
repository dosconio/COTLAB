// ASCII ArinaCovenant
// COPYRIGHT. THE CODE IS CONTRIBUTED TO UNISYM(Apache 2) OF ArinaMgk@GitHub

#ifndef ArnModTensor
#define ArnModTensor
// TENSOR ARITHMETIC and OPERATION(for non-arith-elements)
// Arina Design: end by 0, e.g. 2*2*2(tensor), 3(vector), 2*2(matrix), 0(scalar)
// This inherited the idea.

#include <stddef.h>

typedef struct tensor
{
	size_t* dimen;
	union { void* first; struct tensor* tenfirst; };
} tensor;
/* Nested structure and for example 3*2*0 {
	ln0 {2 col{} {}}
	ln1 {2 col{} {}}
	ln2 {2 col{} {}}
	}
*/

#endif
