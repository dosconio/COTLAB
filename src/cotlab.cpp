// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#define _CRT_SECURE_NO_WARNINGS

#include <cpp/unisym>

#if !defined(_ARINUX) && !defined(_MCCA)

#include <c/file.h>
#include "../inc/cothead.h"
#include "../inc/contask.h"

#define CotTitle "COTLAB Console Build " __DATE__

char ulibver[] = { "Not detected." };// from unisymlib
uni::String uver_str(sliceof(ulibver));

rostr ConMode[]{ "Shell", "File", "Command" };
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
	// ploginfo("Mode   : %s", ConMode[_IMM(option)]);

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
			ctask.Parse();
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

#endif

#ifdef _ARINUX
#include <unistd.h>
#include <sys/wait.h>

_ESYM_C
int _m_waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options, void* rusage);

int main() {
	char command[256];
	while (true) {
		write(1, "# ", sizeof("# ") - 1);// man 2 write
		int count = read(0 _Comment(stdin), command, 255);// man 2 read
		// replace \n into \0
		command[count - 1] = 0;
		pid_t fork_result = fork();
		if (fork_result) {
			// wait children to avoid zombie process
			siginfo_t siginfo;
			_m_waitid(P_ALL, nil, &siginfo, WEXITED, NULL);
		}
		else {
			execve(command, 0, 0);//{} retval?
			break;
		}
	}
	_exit(0);
}

#elif defined(_MCCA)

#include "inc/aaaaa.h"
#include "cpp/queue"

#define EXIT_CODE 0x123
char inbuf[128];
char* argv[25];

int run(char* cmd)
{
	int argc = 0;
	char* p = cmd;

	// Parse the command string in-place
	while (*p) {
		while (*p == ' ') *p++ = '\0';
		if (*p) {
			argv[argc++] = p; // Mark the beginning of an argument
			while (*p && *p != ' ') p++;
		}
		if (argc >= numsof(argv) - 1) break;
	}
	argv[argc] = nullptr; // Null-terminate the argument array for execv
	if (argc == 0) return 0; // Empty command
	// ploginfo("torun: %s (argc=%d)", argv[0], argc);
	int pid = fork();
	if (pid == 0) {
		execv(argv[0], argv);
		// spawnl("/mnt34/apps/d", "/mnt34/apps/d", "a", "b.c", nullptr);
		// execl("/mnt34/apps/d", "/mnt34/apps/d", "a", "b.c", nullptr);
		sysouts("sh: command not found or exec failed.\n\r");
		exit(-1); // Terminate the failed child to prevent dual shells
	}
	else if (pid > 0) {
		// Parent process (Shell): wait for the foreground child to finish
		int s;
		auto child_pid = wait(&s);
		if (child_pid > 0) {
			outsfmt("sh: process %d exited with code %d\n\r", child_pid, s);
		}
	}
	else {
		sysouts("sh: fork failed.\n\r");
	}

	return 0;
}

int main(int argc, char** argv)
{
	//{} startup.cpp
	int fd_inn = sysopen("/dev/tty0");// should-be 0
	int fd_out = sysopen("/dev/tty0");// should-be 1

	int pid = fork();
	if (pid) {
		ploginfo("[Appinit] There is the parent.");
		int s;
		if (int pid2; pid2 = fork()); else {
			sysouts("[Appinit] There is the child's child.\n\r");
			return 1227;
		}
		while (true) {
			int child = wait(&s);
			if (child > 0) {
				outsfmt("[Appinit] %d exited with %d.\n\r", child, s);
			}
			s = 0;
			sysrest();
		}
	}
	else { // here is the shell (primitive COTLAB)
		uni::QueueLimited queue((uni::Slice) { _IMM(inbuf), sizeof(inbuf) });
		int ch;
		ploginfo("[Appinit] Init SHell started.");
		while (true) {
			if ((ch = sysinnc()) > 0) {
				if (ch == '\n') {
					sysouts("\n\r");// run new proc
					char null_term = '\0';
					queue.out(&null_term, 1);
					run(inbuf);
					queue.clear();
				}
				else if (ch == '\b') {
					if (!queue.is_empty()) {
						sysouts("\b \b");
						queue.p--;
					}
				}
				else {
					outsfmt("%c", ch);
					queue.out((char*)&ch, 1);
				}
			}
			else {
				sysrest();
			}
		}
		exit(EXIT_CODE);
	}
}

#endif

