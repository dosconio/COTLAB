// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

//{TODO} Arinux
//{TODO} in Win64 CGW64

#define _CRT_SECURE_NO_WARNINGS
#ifdef _ARINUX
#define _MCCA
#endif
#include <cpp/unisym>

#if !defined(_ACCM) && !defined(_MCCA)// Linux

#include <c/file.h>
#include "../inc/cothead.h"
#include "../inc/contask.h"
#include <unistd.h>
#ifndef _Linux
#include <c/ISO_IEC_STD/signal.h>
#else
#include <signal.h>
#endif

extern int run_command(char* cmd);

#define CotTitle "COTLAB Console Build " __DATE__

char ulibver[] = { "Not detected." };// from unisymlib
uni::String uver_str(sliceof(ulibver));

rostr ConMode[]{ "Shell", "File", "Command" };
enum class option_t { SHELL, FILE, COMMAND };
static option_t option{ option_t::SHELL };

enum prompt_t { COTLAB_PROMPT_FULL, COTLAB_PROMPT_TRIARROW };
static prompt_t prompt{ COTLAB_PROMPT_FULL };

static char inbuf[128];
static char cwd_buf[256];
pureptr_t glb;
bool mode_shell = true;

void print_prompt();

int branch_arith(CotStrBuff& cbuff, IdenChain(&ic_list)[3]) {
	using namespace uni;
	IdenChain& ic_mac = ic_list[0], & ic_sen = ic_list[1], & ic_ise = ic_list[2];
	print_prompt();
	cbuff.getStdin();
	StrDeprefixSpaces(cbuff.reflect());
	if (cbuff == "exit") exit(0);
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
		ctask.Parse();
		_TEMP glb = &ctask;
		if (ctask.Execute()) {
			uni::Nnode* last = ctask.npu->GetNetwork()->Root();
			void* content = stepval(last)->addr;
			ic_sen.Modify("last", CotCopy(content, stepval(last)->type), (datatype)(stepval(last)->type), true);
		}
		ctask.PrintDebug();
	}
	return 0;// important
}



int cotmain(int argc, char** argv) {
	using namespace uni;
	signal(SIGINT, SIG_IGN);
	// Standard I/O (0, 1, 2) is automatically bound to /dev/tty by the kernel
	// write(2, "COTLAB Shell Started\n\r", 22);

	IdenChain ic_list[3];
	for0a(i, ic_list) {
		ic_list[i].chn.refChain().func_free = CotReleaseInode;
	}
	IdenChain& ic_mac = ic_list[0], & ic_sen = ic_list[1], & ic_ise = ic_list[2];

	ic_sen.Modify("last", new Coe(0.0), dt_float, true);
	ic_sen.Modify("pi", new Coe(Coe::pi()), dt_float, false);
	ic_sen.Modify("e", new Coe(Coe::e()), dt_float, false);

	CotStrBuff cbuff;
	{
		cbuff.getEnv("ulibpath").FormatPath() += "Script/Cotoba/version";
		HostFile verfile(cbuff.reference());
		if (bool(verfile)) {
			uver_str.Clear();
			verfile >> uver_str;
		}
	}

	for (stduint i = 1; i < (stduint)argc; i++) {
		if (argv[i][0] == '-') switch (argv[i][1]) {
		case 'c':
			option = option_t::COMMAND; break;
		case 's':// omit the rest of this argv
			option = option_t::SHELL; break;
		case 'f':// follow by a file name
			option = option_t::FILE; break;
		case 'm':
			mode_shell = false; break;
		default:
			printlog(_LOG_ERROR, "Unknown Option: -%s", argv[i]);
			return -1;
		}
	}

	ploginfo("Library: %s", ulibver);
	// ploginfo("Mode   : %s", ConMode[_IMM(option)]);

	//{TODO} implemente Console in UNISYM

	
	switch (option) {
	case option_t::SHELL: while (true) {
		if (mode_shell) {
			if (getcwd(cwd_buf, sizeof(cwd_buf))) {
				outsfmt("root@mach %s > ", cwd_buf);
				fflush(stdout);
			}
			else {
				write(1, "root@mach ? > ", 14);
			}
			int n = read(0, inbuf, sizeof(inbuf) - 1);
			if (n <= 0) break;

			// Kernel handles line editing (\b), we just get the full line
			if (inbuf[n - 1] == '\n') inbuf[n - 1] = '\0';
			else inbuf[n] = '\0';

			if (inbuf[0] != '\0') {
				run_command(inbuf);
			}
		}
		else branch_arith(cbuff, ic_list);
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



#else

#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#ifndef _Linux
#include <c/ISO_IEC_STD/signal.h>
#else
#include <signal.h>
#endif
#include "c/ustring.h"
#include "c/consio.h"

static char inbuf[128];
bool mode_shell = true;

int run_command(char* cmd);

int main(int argc, char** argv) {
	signal(SIGINT, SIG_IGN);
	// Standard I/O (0, 1, 2) is automatically bound to /dev/tty by the kernel
	// write(2, "COTLAB Shell Started\n\r", 22);

	while (true) {
		char cwd_buf[256];
		if (getcwd(cwd_buf, sizeof(cwd_buf))) {
			outsfmt("root@mach %s > ", cwd_buf);
		} else {
			write(1, "root@mach ? > ", 14);
		}
		int n = read(0, inbuf, sizeof(inbuf) - 1);
		if (n <= 0) break;

		// Kernel handles line editing (\b), we just get the full line
		if (inbuf[n - 1] == '\n') inbuf[n - 1] = '\0';
		else inbuf[n] = '\0';

		if (inbuf[0] != '\0') {
			run_command(inbuf);
		}
	}
	return 0;
}

void entry() {
	main(0, nullptr);
}

#ifdef _Linux
extern "C" void OUT_b() {}
extern "C" void IN_b() {}
extern "C" void __stack_chk_fail() { _exit(-2); }
void operator delete(void* ptr, stduint size) noexcept {
}
void operator delete[](void* ptr, stduint size) {
}
uni::OstreamTrait* con0_out = 0;
void outtxt(const char* str, stduint len) {
	write(1, str, len);
}
#endif

#endif