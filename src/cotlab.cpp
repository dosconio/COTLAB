// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

//{TODO} Arinux
//{TODO} in Win64 CGW64

/*example*
	cot -m -c "1+2+3"
*/

#ifdef _ARINUX
#define _MCCA
#endif
#include <cpp/unisym>

#include <c/file.h>
#include "../inc/cothead.h"
#include "../inc/contask.h"
#include <cpp/System/Terminal.hpp>

extern int run_command(char* cmd);
extern bool complete_line(char* buffer, stduint capacity, stduint& length, stduint& position);

#define CotTitle "COTLAB Console Build " __DATE__

char ulibver[] = { "Not detected." };// from unisymlib
uni::String uver_str(sliceof(ulibver));

rostr ConMode[]{ "Shell", "File", "Command" };
enum class option_t { SHELL, FILE, COMMAND };
static option_t option{ option_t::SHELL };

enum prompt_t { COTLAB_PROMPT_FULL, COTLAB_PROMPT_TRIARROW };
static prompt_t prompt{ COTLAB_PROMPT_FULL };

static char inbuf[128];
static char hist_buf[16 * sizeof(inbuf)];

pureptr_t glb;
bool mode_shell = true;

void print_prompt();

int run_arith_line(const char* input, CotStrBuff& cbuff, IdenChain(&ic_list)[3]) {
	using namespace uni;
	IdenChain& ic_mac = ic_list[0], & ic_sen = ic_list[1], & ic_ise = ic_list[2];
	cbuff.uni::String::operator=(input);
	StrDeprefixSpaces(cbuff.reflect());
	if (!cbuff.reference()[0] || cbuff.reference()[0] == '#') return 0;
	if (cbuff == "exit") exit(0);
	else if (cbuff == "+s") {
		mode_shell = true;
	}
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

int run_cot_line(char* input, CotStrBuff& cbuff, IdenChain(&ic_list)[3]) {
	if (!input || input[0] == '\0') return 0;
#ifndef _WinNT
	if (mode_shell) {
		return run_command(input);
	}
#endif
	return run_arith_line(input, cbuff, ic_list);
}

int run_script_file(const char* filepath, CotStrBuff& cbuff, IdenChain(&ic_list)[3]) {
	if (!filepath || !filepath[0]) return -1;
	uni::HostFile file(filepath);
	if (!bool(file)) {
		printlog(_LOG_ERROR, "File %s is misty.", filepath);
		return -1;
	}
	char line_buf[256] = {};
	while (file.FetchLine(line_buf, sizeof(line_buf))) {
		stduint len = StrLength(line_buf);
		while (len > 0 && (line_buf[len - 1] == '\n' || line_buf[len - 1] == '\r')) {
			line_buf[--len] = '\0';
		}
		if (line_buf[0] != '\0') {
			run_cot_line(line_buf, cbuff, ic_list);
		}
		line_buf[0] = '\0';
	}
	return 0;
}

int branch_arith(CotStrBuff& cbuff, IdenChain(&ic_list)[3], uni::TerminalLine& line) {
	print_prompt();
	if (!line.Edit()) return 1;
	if (inbuf[0] != '\0') {
		line.PushHistory(inbuf);
		run_arith_line(inbuf, cbuff, ic_list);
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

	const char* target_arg = nullptr;
	for (int i = 1; i < argc; i++) {
		if (!argv || !argv[i]) continue;
		if (argv[i][0] == '-') switch (argv[i][1]) {
		case 'c':
			option = option_t::COMMAND;
			if (argv[i][2] != '\0') target_arg = &argv[i][2];
			break;
		case 's':// omit the rest of this argv
			option = option_t::SHELL; break;
		case 'f':// follow by a file name
			option = option_t::FILE;
			if (argv[i][2] != '\0') target_arg = &argv[i][2];
			break;
		case 'm':
			mode_shell = false; break;
		default:
			if (option == option_t::COMMAND && !target_arg) {
				target_arg = argv[i];
				break;
			}
			printlog(_LOG_ERROR, "Unknown Option: %s", argv[i]);
			return -1;
		}
		else {
			if (option == option_t::SHELL) {
				option = option_t::FILE;
			}
			target_arg = argv[i];
		}
	}
	if (option == option_t::COMMAND && !target_arg) {
		printlog(_LOG_ERROR, "Missing command for -c");
		return -1;
	}
	if (option == option_t::FILE && !target_arg) {
		printlog(_LOG_ERROR, "Missing file name for -f");
		return -1;
	}
	// ploginfo("Mode   : %s", ConMode[_IMM(option)]);

	//{TODO} implemente Console in UNISYM

#ifdef _WinNT
	mode_shell = false;
#endif
	switch (option) {
	case option_t::SHELL: {
		ploginfo("Library: %s", ulibver);
		uni::TerminalBackend backend;
		uni::TerminalDecoder decoder(&backend);
		uni::TerminalLine line(&decoder, inbuf, sizeof(inbuf), hist_buf, sizeof(hist_buf) / sizeof(inbuf), complete_line);
		while (true) {
			if (mode_shell) {
#ifndef _WinNT
				print_prompt();
				if (!line.Edit()) break;

				if (inbuf[0] != '\0') {
					line.PushHistory(inbuf);
					run_command(inbuf);
				}
#endif
			}
			else if (branch_arith(cbuff, ic_list, line)) break;
		}
	} break;
	case option_t::FILE:
		return run_script_file(target_arg, cbuff, ic_list);
	case option_t::COMMAND: {
		char cmd_buf[256] = {};
		if (target_arg) StrCopyN(cmd_buf, target_arg, sizeof(cmd_buf) - 1);
		return run_cot_line(cmd_buf, cbuff, ic_list);
	}
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


// #ifdef _Linux
// extern "C" void OUT_b() {}
// extern "C" void IN_b() {}
// extern "C" void __stack_chk_fail() { _exit(-2); }
// void operator delete(void* ptr, stduint size) noexcept {
// }
// void operator delete[](void* ptr, stduint size) {
// }
// uni::OstreamTrait* con0_out = 0;
// void outtxt(const char* str, stduint len) {
// 	write(1, str, len);
// }
// #endif
