// ASCII CPP-14 TAB4 CRLF
// AllAuthor: @dosconio
// ModuTitle: COTLAB Console
// Copyright: Dosconio COTLAB, GNU-GPL Version 3

#ifdef _ARINUX
#define _MCCA
#endif

#include "../inc/coenvir.h"
#include "../inc/cothead.h"
#include "../inc/contask.h"


extern "C" char* getenv(const char* name);
extern "C" int setenv(const char* name, const char* value, int overwrite);
extern bool mode_shell;

static char* argv[25];

#ifndef _WinNT

void print_prompt() {
	CotStrBuff sb(0x100);
	#ifndef _MCCA //{}TEMP
	CotStrBuff username;
	CotStrBuff hostname;
	username.getEnv("USER"
		#ifdef _WinNT
		"NAME"// %USERNAME%
		#endif
	);
	username.Refresh();
	//
	#ifdef _WinNT
	hostname.getEnv("COMPUTERNAME");
	#else
	hostname.getEnv("HOST");
	#endif
	hostname.Refresh();
	uni::Console.OutFormat("%s@%s %s %c ",
		username.getCharCount() ? username.reference() : "root",
		hostname.getCharCount() ? hostname.reference() : "localhost",
		sb.getCwd().FormatPath().reference(),
		mode_shell ? '$' : '>' // $# and >
	);
	fflush(stdout);
	#endif
}

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
			path_env = "/md0"; // "/md0:/mnt34/apps";
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

bool complete_line(char* buffer, stduint capacity, stduint& length, stduint& position) {
	stduint begin = position;
	while (begin && buffer[begin - 1] != ' ') begin--;
	stduint word = position - begin;

	bool cd_arg = false;
	{
		stduint p = begin;
		while (p && buffer[p - 1] == ' ') p--;
		stduint prev_begin = p;
		while (prev_begin && buffer[prev_begin - 1] != ' ') prev_begin--;
		if (p - prev_begin == 2 && buffer[prev_begin] == 'c' && buffer[prev_begin + 1] == 'd') cd_arg = true;
	}

	stduint slash = 0;
	for (stduint i = 0; i < word; i++) {
		if (buffer[begin + i] == '/') slash = i + 1;
	}
	if (!slash && !cd_arg) return false;

	char dir[256];
	stduint dir_len = 0;
	if (slash) {
		for (stduint i = 0; i < slash && dir_len + 1 < sizeof(dir); i++) dir[dir_len++] = buffer[begin + i];
	}
	else {
		dir[dir_len++] = '.';
		dir[dir_len++] = '/';
	}
	dir[dir_len] = 0;

	char prefix[128];
	stduint prefix_len = 0;
	for (stduint i = slash; i < word && prefix_len + 1 < sizeof(prefix); i++) prefix[prefix_len++] = buffer[begin + i];
	prefix[prefix_len] = 0;

	DIR* d = opendir(dir);
	if (!d) return false;

	char common[128];
	stduint common_len = 0;
	stduint hits = 0;
	struct dirent* entry;
	while ((entry = readdir(d)) != nullptr) {
		const char* name = entry->d_name;
		if (StrCompare(name, ".") == 0 || StrCompare(name, "..") == 0) continue;
		if (StrCompareN(name, prefix, prefix_len) != 0) continue;
		if (cd_arg) {
			char full[512];
			StrCopy(full, dir);
			StrAppend(full, name);
			struct stat st;
			if (stat(full, &st) != 0 || !(st.st_mode & S_IFDIR)) continue;
		}
		if (!hits) {
			StrCopyN(common, name, sizeof(common));
			common_len = StrLength(common);
		}
		else {
			stduint i = 0;
			while (i < common_len && name[i] == common[i]) i++;
			common_len = i;
			common[i] = 0;
		}
		hits++;
	}
	closedir(d);
	if (!hits) return false;
	if (hits > 1 && common_len <= prefix_len) return false;

	stduint base = begin + slash;
	if (base + common_len >= capacity) return false;
	for (stduint i = 0; i < common_len; i++) buffer[base + i] = common[i];
	length = base + common_len;
	position = length;

	bool is_dir = false;
	if (hits == 1) {
		char full[512];
		StrCopy(full, dir);
		StrAppend(full, common);
		struct stat st;
		if (stat(full, &st) == 0 && (st.st_mode & S_IFDIR)) is_dir = true;
	}
	if (is_dir && length + 1 < capacity) {
		buffer[length++] = '/';
	}
	else if (!is_dir && hits == 1 && length + 1 < capacity) {
		buffer[length++] = ' ';
	}
	position = length;
	buffer[length] = 0;
	return true;
}

int run_command(char* cmd) {
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
	else if (StrCompare("+m", argv[0]) == 0) {
		mode_shell = false;
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

#else

void print_prompt() {
	printf("> ");
}

#endif

