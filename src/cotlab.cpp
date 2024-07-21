// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cpp/console>
#include "../inc/cothead.h"

#include "c/debug.h"

#include <cpp/unisym>//{TEMP}

extern "C" {
	stduint _MALCOUNT = 0;
	stduint _MALLIMIT = 0x1000;
	stduint call_state = 0x1000;
}


#define CotTitle "COTLAB Console Build " __DATE__ "\n"

const char* ConMode[]{
	"Shell", "File", "Command"
};

char ulibver[16 + 1] = { "Not detected." };// from unisymlib

enum option_t { COTLAB_OPTION_SHELL, COTLAB_OPTION_FILE, COTLAB_OPTION_COMMAND } option{ COTLAB_OPTION_COMMAND };
enum prompt_t { COTLAB_PROMPT_FULL, COTLAB_PROMPT_TRIARROW } prompt{ COTLAB_PROMPT_FULL };


// RAII Group
struct CotStrBuff : public uni::String {
	CotStrBuff(stduint lenbuf = 0) : String(lenbuf) {
	}
	stduint getStdin() { return ConScanLine(addr, counts); }
	CotStrBuff& getEnv(const char* str) { self.String::operator=(getenv(str)); return self; }
	CotStrBuff& getCwd() { (void)ConGetCurrentDirectory(addr, limits); return self; }
	CotStrBuff& FormatPath() {
		_TEMP // with suffix
			for0(i, StrLength(addr)) if (addr[i] == '\\') 
				addr[i] = '/';
		if (StrCharLast(addr) != '/') uni::String::operator+=("/");
		return self;
	}
};
struct CotFile {
	CotFile(const uni::String& fname, const char* mode = "r") { pf = fopen(fname.reference(), mode); }
	~CotFile() { fclose(pf); pf = nullptr; }
	operator FILE* () { return pf; }
	bool getc(char& ref) { int ch = fgetc(pf); ref = (ch == EOF) ? 0 : (char)ch; return (ch != EOF); }
protected:
	FILE* pf;
};
pureptr_t glb;
int cotmain(int argc, char** argv) {

	stduint i = 0;

	IdenChain ic_list[3];
	IdenChain& ic_mac = ic_list[0], & ic_sen = ic_list[1], & ic_ise = ic_list[2];

	ic_sen.Modify("last", new uni::Coe(0.0), dt_float, true);
	ic_sen.Modify("pi", new uni::Coe(uni::Coe::pi()), dt_float, false);
	ic_sen.Modify("e", new uni::Coe(uni::Coe::e()), dt_float, false);

	CotStrBuff cbuff;
	cbuff.getEnv("ulibpath").FormatPath() += "Script/Cotoba/version";
	for (CotFile verfile(cbuff); i < sizeof(ulibver) - 1 &&
		(verfile.getc(ulibver[i])); i++);
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') switch (argv[i][1]) {
		case 's':// omit the rest of this argv
			option = COTLAB_OPTION_SHELL;
			break;
		case 'f':// follow by a file name
			option = COTLAB_OPTION_FILE;
			break;
		default:
			printf("Unknown Option: %s\n", argv[i]);
			return -1;
		}
	}
	printf("Library: %s\n", ulibver);
	printf("Mode   : %s\n", ConMode[option]);
	
	// ConStyleNormal();// <- use this for IO
	//{TODO} implemente Console in UNISYM

	switch (option) {
	case COTLAB_OPTION_SHELL: while (true) {
		using namespace uni;
		i = 0;
		{
			CotStrBuff sb(0x100);
			std::cout << sb.getCwd().FormatPath() << "> ";
			cbuff.getStdin();
			StrDeprefixSpaces(cbuff.reflect());
		}
		if (cbuff == "exit") break;
		else if (cbuff == "help") {
			printf(
				"\nCOTLAB: Her SGA-4 (doscon.io), GNU GPL-3 (cotlab.org)"
				"\n   ||"
				"\n   |+ Doshou Haruno [Dscn.Org.Chief] (dosconyo@gmail.com)"
				"\n   |+ Ren    Phina  [None.Org.Advis] (   phina@ tuta.io )"
				"\n   + UNISYM: %s\n", ulibver);
			printf("Language: COTOBA 4.1\n");
		}
		else if (cbuff == "list") {
			InodePrint(&ic_mac);
			InodePrint(&ic_sen);
			InodePrint(&ic_ise);
		}
		else if (cbuff == "cls") Console::Clear();
		else if (!StrCompareN(cbuff.reference(), "cd ", 3)) {
			if (ConSetCurrentDirectory(cbuff.reference() + 3))
				fprintf(stderr, "Invaild Working Path.\n");
		}
		//{TODO} A single identifier which is not a valuable will be a calling
		else {
			Contask ctask(cbuff.reflect(), CONTASK_BUF, ic_list);//{TODO} task-pool and peculiar function
			ctask.Prep(); 
			//ctask.PrintDebug(); puts("");
			ctask.Parse();
			//ctask.PrintDebug(); puts(""); 
			_TEMP glb = &ctask;
			if (ctask.Execute()) {
				uni::Nnode* last = ctask.npu->GetNetwork()->Root();
				void* content = stepval(last)->addr;
				ic_sen.Modify("last", CotCopy(content, stepval(last)->type), (datatype)(stepval(last)->type), true);
			}
			ctask.PrintDebug();
		}
	} break;
	case COTLAB_OPTION_FILE:
		//{TODO}
		//if (!(fp = fopen(point, "r")))
		//{
		//	printf("File %s is misty.", (char*)point);
		//	goto endo;
		//}
		break;
	case COTLAB_OPTION_COMMAND:
		//{CANCELLED}
		break;
	default:
		printf("Unknown Option: %d\n", option);//# inside error
		break;
	}

	return 0;
}

int main(int argc, char** argv) {
	int retno = cotmain(argc, argv);
	if (_MALCOUNT)
		printf("\nMemory Leak: 0x%" PRIxSTD " !\n", _MALCOUNT);
	return retno;
}

void entry() {
	//TEMP for Mecocoa and other free-standing environments
	cotmain(0, 0);
}
