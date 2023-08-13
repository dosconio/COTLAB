#ifndef HrnCotlabToken
#define HrnCotlabToken

typedef double num;
typedef struct tensor
{
	size_t* dimen;// end by 0, e.g. 2*2*2, 3(vector), 2*2(matrix), 0(scalar)
	union { num* first; struct tensor* tenfirst; };
	/*
	nested: e.g. 3*2 matrix dim:3,2,0
	ln0 {2 col{} {}}
	ln1 {2 col{} {}}
	ln2 {2 col{} {}}
	*/
} tensor;//TODO. can be made into unisym
// TODO. _nobuf na coear etc.
enum datatype
{
	dt_sint,// > uint
	dt_uint,
	dt_float,// coe*
	dt_num,// num*
	dt_str,// char*
	dt_tensor,// T[] ten* > matrix > vector
	dt_rangei,// [ ~ ]
	dt_rangec,// [ , ]
	dt_matrix,// M[]
	dt_vector,// V[]
	dt_readonly,// Ro[] Tuple in Python
	dt_comb,// [] List in Python. (type of combination can be different)
	dt_ary,// dt_x[]
	dtx_dictionary// from Python
};

typedef struct varilist
{
	enum datatype ptype;
	void* content;
	struct funcpara* next;
} vlist;

typedef struct Todir
{
	char* fname;
	vlist* fargu;
	struct scope* parent;
	struct scope* subfirst;
	struct scope* left, * right;
	void* res;
} Todir;




//TODO. typedef fpara* (* const eletype)(fpara*);



#endif