// ASCII
#include <iostream>
#include "number"
extern "C" 
{
	#include <aldbg.h>
	#include <stdlib.h>
	#include <ustring.h>
}

extern "C"
{
	void erro(char* errmsg)
	{
		std::cerr << errmsg;
		std::cout << std::endl << "Sayounara~"
#ifdef _dbg
			<< malc_count
#endif
			<< std::endl;
		exit(1);
	}
	char arna_tempor[64];
	char arna_tmpslv[sizeof arna_tempor], arna_tmpext[sizeof arna_tempor];
	size_t malc_count, malc_occupy, malc_limit = sizeof arna_tempor;
	size_t show_precise = 64, malc_precise = 64;
	size_t lup_times = 16;// Input: minimum times.
	size_t lup_limit = 100;// Input to mold
	size_t lup_last;// Output to here
}

int main()
{
	arna_eflag.Signed = 1;
	arna_eflag.HeapYo = 1;
mainx:
	{
		using namespace std;
		number num{ 1.0,2.0 };
		number num2 = 2.2;
		number num3 = 0.2;
		number num4 = 20;
		// char* str = num.RealToString();
		
		cout << "1.0`2.0++ == " << (num++).ToString() << endl;
		cout << "--2.2 == " << (--num2).ToString() << endl;
		cout << "num ==" << (num + num2 - num2).ToString() << endl;
		cout << "num2 ==" << (num2 - 1.2 + 1.2).ToString() << endl;
		cout << "1+num2*2/1+3 ==" << (1 + num2 * 2 / 1 + 3).ToString() << endl;
		cout << "num3 ==" << num3.ToString() << endl;
		cout << "num4 ==" << num4.ToString() << endl;
	}
	erro((char*)"Sayounara! - Erro demonstration.");
	return 0;
}

