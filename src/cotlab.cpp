// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#define _CRT_SECURE_NO_WARNINGS

#include <c/file.h>
#include "../inc/cothead.h"
#include "../inc/contask.h"

#define CotTitle "COTLAB Console Build " __DATE__

char ulibver[16 + 1] = { "Not detected." };// from unisymlib
uni::String uver_str(sliceof(ulibver));

rostr ConMode[]{ "Shell", "File", "Command"};
enum class option_t { SHELL, FILE, COMMAND };
static option_t option{ option_t::SHELL };

enum prompt_t { COTLAB_PROMPT_FULL, COTLAB_PROMPT_TRIARROW };
static prompt_t prompt{ COTLAB_PROMPT_FULL };

struct CotStrBuff : public uni::String {
	CotStrBuff(stduint lenbuf = 0x1000) : String(lenbuf) { }
	stduint getStdin() { 
		return ConScanLine(addr, limits);
	}
	CotStrBuff& getEnv(const char* str) { 
		StrCopy(addr, getenv(str));// self.String::operator=(getenv(str));
		return self; 
	}
	CotStrBuff& getCwd() { (void)ConGetCurrentDirectory(addr, limits); return self; }
	CotStrBuff& FormatPath() { // with suffix
		//{TODO} Replace
		uni::String& the_str = self;
		Replaced("\\", "/");
		if (self[-1] != '/') self += "/";
		return self;
	}
};


pureptr_t glb;
int cotmain(int argc, char** argv) {
	using namespace uni;

	IdenChain ic_list[3];
	IdenChain& ic_mac = ic_list[0], & ic_sen = ic_list[1], & ic_ise = ic_list[2];

	ic_sen.Modify("last", new Coe(0.0), dt_float, true);
	ic_sen.Modify("pi", new Coe(Coe::pi()), dt_float, false);
	ic_sen.Modify("e", new Coe(Coe::e()), dt_float, false);

	CotStrBuff cbuff;
	{
		cbuff.getEnv("ulibpath").FormatPath() += "Script/Cotoba/version";
		HostFile verfile(cbuff.reference());
		if (bool(verfile)) verfile >> uver_str;
	}
	for (stduint i = 1; i < argc; i++) {
		if (argv[i][0] == '-') switch (argv[i][1]) {
		case 'c':
			option = option_t::COMMAND; break;
		case 's':// omit the rest of this argv
			option = option_t::SHELL; break;
		case 'f':// follow by a file name
			option = option_t::FILE; break;
		default:
			printlog(_LOG_ERROR, "Unknown Option: -%s", argv[i]);
			return -1;
		}
	}
	ploginfo("Library: %s", ulibver);
	ploginfo("Mode   : %s", ConMode[_IMM(option)]);
	
	//{TODO} implemente Console in UNISYM

	switch (option) {
	case option_t::SHELL: while (true) {
		{
			CotStrBuff sb(0x100);
			Console.OutFormat("%s> ", sb.getCwd().FormatPath().reference());
			cbuff.getStdin();
			StrDeprefixSpaces(cbuff.reflect());
		}
		if (cbuff == "exit") break;
		else if (cbuff == "help") {
			Console.OutFormat(
				CotTitle
				"\nCOTLAB: Her SGA-4 (doscon.io), GPL-3 (cotlab.org)"
				"\n   ||"
				"\n   |+ Doshou Haruno [Dscn.Org.Chief] (dosconyo@gmail.com)"
				"\n   |+ Ren    Phina  [None.Org.Advis] (   phina@ tuta.io )"
				"\n   + UNISYM: %s\n", ulibver);
			Console.OutFormat(
				"Language: COTOBA 0.0.3\n"
			);
		}
		else if (cbuff == "list") {
			InodePrint(&ic_mac);
			InodePrint(&ic_sen);
			InodePrint(&ic_ise);
		}
		else if (cbuff == "cls") Console.Clear();
		else if (!StrCompareN(cbuff.reference(), "cd ", 3)) {
			if (ConSetCurrentDirectory(cbuff.reference() + 3))
				printlog(_LOG_ERROR, "Invaild Working Path.");
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
	case option_t::FILE:
		//{TODO}
		//if (!(fp = fopen(point, "r")))
		//{
		//	printf("File %s is misty.", (char*)point);
		//	goto endo;
		//}
		break;
	case option_t::COMMAND:
		//{CANCELLED}
		break;
	default:
		printlog(_LOG_ERROR, "Unknown Option: %d", option);//# inside error
		break;
	}

	return 0;
}

int main(int argc, char** argv) {
	int retno = cotmain(argc, argv);
	if (_MALCOUNT)
		printlog(_LOG_ERROR, "\nMemory Leak: %[u].", _MALCOUNT);
	return retno;
}

void entry() {
	//TEMP for Mecocoa and other free-standing environments
	cotmain(0, 0);
}
