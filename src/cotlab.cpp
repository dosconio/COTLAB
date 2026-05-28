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
	CotStrBuff(stduint lenbuf = 0x1000) : String(String::Charset::UTF8, lenbuf) { }
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
static char* argv[25];

extern "C" char* getenv(const char* name);
extern "C" int setenv(const char* name, const char* value, int overwrite);

static bool resolve_path(const char* cmd, char* resolved, size_t max_len) {
	bool has_slash = false;
	const char* path_ptr = cmd;
	while (*path_ptr) {
		if (*path_ptr == '/') {
			has_slash = true;
			break;
		}
		path_ptr++;
	}

	if (!has_slash) {
		const char* path_env = getenv("PATH");
		if (!path_env) {
			path_env = "/md0:/mnt34/apps";
		}
		char path_buf[256];
		StrCopyN(path_buf, path_env, sizeof(path_buf));
		char* dir = path_buf;
		while (dir && *dir) {
			char* next_dir = (char*)StrIndexChar(dir, ':');
			if (next_dir) {
				*next_dir = '\0';
				next_dir++;
			}
			
			StrCopy(resolved, dir);
			stduint rlen = StrLength(resolved);
			if (rlen > 0 && resolved[rlen - 1] != '/') {
				StrAppend(resolved, "/");
			}
			StrAppend(resolved, cmd);
			
			int fd = open(resolved, O_RDONLY);
			if (fd >= 0) {
				close(fd);
				return true;
			}
			dir = next_dir;
		}
	} else {
		StrCopyN(resolved, cmd, max_len);
		return true;
	}
	return false;
}

static int run(char* cmd) {
	static char expanded[512];
	const char* src = cmd;
	char* dst = expanded;
	char* dst_end = expanded + sizeof(expanded) - 4;

	// Preprocess to insert spaces around redirection and pipe operators
	while (*src && dst < dst_end) {
		if (src[0] == '>' && src[1] == '>') {
			if (dst > expanded && dst[-1] != ' ') {
				*dst++ = ' ';
			}
			*dst++ = '>';
			*dst++ = '>';
			src += 2;
			if (*src && *src != ' ') {
				*dst++ = ' ';
			}
		} else if (src[0] == '2' && src[1] == '>') {
			if (dst > expanded && dst[-1] != ' ') {
				*dst++ = ' ';
			}
			*dst++ = '2';
			*dst++ = '>';
			src += 2;
			if (*src && *src != ' ') {
				*dst++ = ' ';
			}
		} else if (src[0] == '>') {
			if (dst > expanded && dst[-1] != ' ') {
				*dst++ = ' ';
			}
			*dst++ = '>';
			src += 1;
			if (*src && *src != ' ') {
				*dst++ = ' ';
			}
		} else if (src[0] == '<') {
			if (dst > expanded && dst[-1] != ' ') {
				*dst++ = ' ';
			}
			*dst++ = '<';
			src += 1;
			if (*src && *src != ' ') {
				*dst++ = ' ';
			}
		} else if (src[0] == '|') {
			if (dst > expanded && dst[-1] != ' ') {
				*dst++ = ' ';
			}
			*dst++ = '|';
			src += 1;
			if (*src && *src != ' ') {
				*dst++ = ' ';
			}
		} else {
			*dst++ = *src++;
		}
	}
	*dst = '\0';

	unsigned argc = 0;
	char* p = expanded;

	// Parse the command string in-place
	while (*p) {
		while (*p == ' ') *p++ = '\0';
		if (*p) {
			argv[argc++] = p;
			while (*p && *p != ' ') p++;
		}
		if (argc >= (sizeof(argv) / sizeof(argv[0])) - 1) break;
	}
	argv[argc] = nullptr;
	if (argc == 0) return 0;

	// Parse redirection arguments in parsed argv
	const char* infile = nullptr;
	const char* outfile = nullptr;
	const char* errfile = nullptr;
	bool append_out = false;
	unsigned new_argc = 0;

	for (unsigned i = 0; i < argc; i++) {
		if (StrCompare(argv[i], "<") == 0) {
			if (i + 1 < argc) {
				infile = argv[i + 1];
				i++; // Skip filename
			} else {
				write(2, "sh: syntax error near unexpected token `newline'\n\r", 50);
				return -1;
			}
		} else if (StrCompare(argv[i], ">") == 0) {
			if (i + 1 < argc) {
				outfile = argv[i + 1];
				append_out = false;
				i++;
			} else {
				write(2, "sh: syntax error near unexpected token `newline'\n\r", 50);
				return -1;
			}
		} else if (StrCompare(argv[i], ">>") == 0) {
			if (i + 1 < argc) {
				outfile = argv[i + 1];
				append_out = true;
				i++;
			} else {
				write(2, "sh: syntax error near unexpected token `newline'\n\r", 50);
				return -1;
			}
		} else if (StrCompare(argv[i], "2>") == 0) {
			if (i + 1 < argc) {
				errfile = argv[i + 1];
				i++;
			} else {
				write(2, "sh: syntax error near unexpected token `newline'\n\r", 50);
				return -1;
			}
		} else {
			argv[new_argc++] = argv[i];
		}
	}
	argv[new_argc] = nullptr;
	argc = new_argc;

	// Scan for pipe symbol '|' in argv
	int pipe_idx = -1;
	for (unsigned i = 0; i < argc; i++) {
		if (StrCompare(argv[i], "|") == 0) {
			pipe_idx = (int)i;
			break;
		}
	}

	if (pipe_idx >= 0) {
		// Split command into left (A) and right (B)
		argv[pipe_idx] = nullptr;
		char** argv_left = argv;
		char** argv_right = &argv[pipe_idx + 1];

		int pipefd[2];
		if (pipe(pipefd) < 0) {
			write(2, "sh: pipe creation failed\n\r", 26);
			return -1;
		}

		// Fork left process (Command A)
		pid_t pid_left = fork();
		if (pid_left == 0) {
			signal(SIGINT, SIG_DFL);
			dup2(pipefd[1], 1); // Redirect stdout to pipe write end
			close(pipefd[0]);   // Close unused read end
			close(pipefd[1]);

			// Apply input redirection if specified for left process
			if (infile) {
				int fd = open(infile, O_RDONLY);
				if (fd < 0) {
					write(2, "sh: failed to open input file\n\r", 31);
					_exit(-1);
				}
				dup2(fd, 0);
				close(fd);
			}

			char resolved_path_left[256];
			if (!resolve_path(argv_left[0], resolved_path_left, sizeof(resolved_path_left))) {
				write(2, "sh: command not found\n\r", 23);
				_exit(-1);
			}

			execv(resolved_path_left, argv_left);
			_exit(-1);
		}

		// Fork right process (Command B)
		pid_t pid_right = fork();
		if (pid_right == 0) {
			signal(SIGINT, SIG_DFL);
			dup2(pipefd[0], 0); // Redirect stdin to pipe read end
			close(pipefd[0]);
			close(pipefd[1]);   // Close unused write end

			// Apply output redirection if specified for right process
			if (outfile) {
				int flags = O_WRONLY | O_CREAT;
				if (append_out) {
					flags |= O_APPEND;
				} else {
					flags |= O_TRUNC;
				}
				int fd = open(outfile, flags);
				if (fd < 0) {
					write(2, "sh: failed to open output file\n\r", 32);
					_exit(-1);
				}
				dup2(fd, 1);
				close(fd);
			}

			// Apply stderr redirection if specified for right process
			if (errfile) {
				int fd = open(errfile, O_WRONLY | O_CREAT | O_TRUNC);
				if (fd < 0) {
					write(2, "sh: failed to open error file\n\r", 31);
					_exit(-1);
				}
				dup2(fd, 2);
				close(fd);
			}

			char resolved_path_right[256];
			if (!resolve_path(argv_right[0], resolved_path_right, sizeof(resolved_path_right))) {
				write(2, "sh: command not found\n\r", 23);
				_exit(-1);
			}

			execv(resolved_path_right, argv_right);
			_exit(-1);
		}

		// Shell parent process must close its own descriptors
		close(pipefd[0]);
		close(pipefd[1]);

		// Wait for both child processes to complete
		int status_left = 0, status_right = 0;
		wait(&status_left);
		wait(&status_right);
		
		// Update status code environment variable using right process exit status
		char status_str[16];
		outsfmtbuf(status_str, "%d", status_right);
		setenv("?", status_str, 1);
		return 0;
	}

	// Built-in command: exit
	if (StrCompare("exit", argv[0]) == 0) {
		int retcode = (argc > 1) ? atoins(argv[1]) : 0;
		_exit(retcode);
		return retcode;
	}

	// Built-in command: cd
	if (StrCompare("cd", argv[0]) == 0) {
		const char* target_path = (argc > 1) ? argv[1] : "/";
		if (chdir(target_path) < 0) {
			write(2, "cd: failed to change directory\n\r", 32);
		}
		return 0;
	}

	// Built-in command: cot+ls (simple directory listing)
	if (StrCompare("cot+ls", argv[0]) == 0) {
		const char* path = (argc > 1) ? argv[1] : ".";
		DIR* dir = opendir(path);
		if (!dir) {
			struct stat st;
			if (stat(path, &st) == 0) {
				outsfmt("%s\n\r", path);
				return 0;
			}
			write(2, "ls: cannot access path\n\r", 24);
			return -1;
		}
		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			outsfmt("%s  ", entry->d_name);
		}
		outsfmt("\n\r");
		closedir(dir);
		return 0;
	}

	char resolved_path[256];
	if (!resolve_path(argv[0], resolved_path, sizeof(resolved_path))) {
		write(2, "sh: command not found\n\r", 23);
		setenv("?", "127", 1);
		return -1;
	}

	pid_t pid = fork();
	if (pid == 0) {
		signal(SIGINT, SIG_DFL);

		// Apply input redirection
		if (infile) {
			int fd = open(infile, O_RDONLY);
			if (fd < 0) {
				write(2, "sh: failed to open input file\n\r", 31);
				_exit(-1);
			}
			dup2(fd, 0);
			close(fd);
		}

		// Apply output redirection
		if (outfile) {
			int flags = O_WRONLY | O_CREAT;
			if (append_out) {
				flags |= O_APPEND;
			} else {
				flags |= O_TRUNC;
			}
			int fd = open(outfile, flags);
			if (fd < 0) {
				write(2, "sh: failed to open output file\n\r", 32);
				_exit(-1);
			}
			dup2(fd, 1);
			close(fd);
		}

		// Apply stderr redirection
		if (errfile) {
			int fd = open(errfile, O_WRONLY | O_CREAT | O_TRUNC);
			if (fd < 0) {
				write(2, "sh: failed to open error file\n\r", 31);
				_exit(-1);
			}
			dup2(fd, 2);
			close(fd);
		}

		execv(resolved_path, argv);
		// If execv returns, it failed
		write(2, "sh: command not found\n\r", 23);
		_exit(-1);
	} else if (pid > 0) {
		int status;
		pid_t child_pid = wait(&status);
		if (child_pid > 0) {
			if (status) outsfmt("sh: process %d exited with code %d\n\r", child_pid, status);
			
			// Update exit status code environment variable
			char status_str[16];
			outsfmtbuf(status_str, "%d", status);
			setenv("?", status_str, 1);
		}
	} else {
		write(2, "sh: fork failed\n\r", 17);
	}
	return 0;
}


int main(int argc, char** argv) {
	signal(SIGINT, SIG_IGN);
	// Standard I/O (0, 1, 2) is automatically bound to /dev/tty by the kernel
	write(2, "COTLAB Shell Started\n\r", 22);

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
			run(inbuf);
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

