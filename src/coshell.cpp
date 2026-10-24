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

class ShellStrInn : public uni::IstreamTrait {
	const char* ptr;
public:
	ShellStrInn(const char* s) : ptr(s ? s : "") {}
	virtual int inn() {
		return (*ptr) ? (unsigned char)*ptr++ : EOF;
	}
};

enum class RedirKind {
	None,
	In,
	OutTrunc,
	OutAppend,
	Err
};

struct CommandStage {
	char* argv[16];
	unsigned argc;
	const char* infile;
	const char* outfile;
	const char* errfile;
	bool append_out;
};

static constexpr unsigned MAX_PIPE_STAGES = 8;
static CommandStage stages[MAX_PIPE_STAGES];

static int builtin_cot_ls(unsigned argc, char** stage_argv) {
	const char* path = (argc > 1) ? stage_argv[1] : ".";
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

int run_command(char* cmd) {
	static char expanded[512];
	char* dst = expanded;
	char* dst_end = expanded + sizeof(expanded) - 4;
	constexpr unsigned max_stage_argc = (sizeof(stages[0].argv) / sizeof(stages[0].argv[0])) - 1;

	unsigned stage_count = 1;
	MemSet(stages, 0, sizeof(stages));

	char* current_word = nullptr;
	bool in_word = false;
	RedirKind pending_redir = RedirKind::None;
	bool syntax_error = false;
	const char* syntax_err_msg = "sh: syntax error near unexpected token `newline'\n\r";

	auto close_word = [&]() {
		if (!in_word) return;
		*dst++ = '\0';
		in_word = false;
		CommandStage& st = stages[stage_count - 1];
		switch (pending_redir) {
		case RedirKind::None:
			if (st.argc < max_stage_argc) {
				st.argv[st.argc++] = current_word;
			}
			break;
		case RedirKind::In:
			st.infile = current_word;
			pending_redir = RedirKind::None;
			break;
		case RedirKind::OutTrunc:
			st.outfile = current_word;
			st.append_out = false;
			pending_redir = RedirKind::None;
			break;
		case RedirKind::OutAppend:
			st.outfile = current_word;
			st.append_out = true;
			pending_redir = RedirKind::None;
			break;
		case RedirKind::Err:
			st.errfile = current_word;
			pending_redir = RedirKind::None;
			break;
		}
		current_word = nullptr;
	};

	auto append_text = [&](const char* s, stduint len) {
		if (syntax_error) return;
		if (!in_word) {
			if (dst >= dst_end) return;
			current_word = dst;
			in_word = true;
		}
		while (len-- && dst < dst_end) {
			*dst++ = *s++;
		}
	};

	auto begin_redir = [&](RedirKind kind) {
		if (syntax_error) return;
		close_word();
		if (pending_redir != RedirKind::None) {
			syntax_error = true;
			return;
		}
		pending_redir = kind;
	};

	auto begin_next_stage = [&]() {
		if (syntax_error) return;
		close_word();
		if (pending_redir != RedirKind::None || stages[stage_count - 1].argc == 0) {
			syntax_error = true;
			syntax_err_msg = "sh: syntax error near unexpected token `|'\n\r";
			return;
		}
		stages[stage_count - 1].argv[stages[stage_count - 1].argc] = nullptr;
		if (stage_count >= MAX_PIPE_STAGES) {
			syntax_error = true;
			syntax_err_msg = "sh: too many pipe stages\n\r";
			return;
		}
		stage_count++;
	};

	auto parse_symbol_span = [&](const char* s) {
		while (*s && !syntax_error) {
			if (s[0] == '>' && s[1] == '>') {
				begin_redir(RedirKind::OutAppend);
				s += 2;
			}
			else if (s[0] == '>') {
				begin_redir(RedirKind::OutTrunc);
				s += 1;
			}
			else if (s[0] == '<') {
				begin_redir(RedirKind::In);
				s += 1;
			}
			else if (s[0] == '|') {
				begin_next_stage();
				s += 1;
			}
			else {
				append_text(s, 1);
				s += 1;
			}
		}
	};

	// Tokenize command line via LinearParser without NestedParseUnit
	ShellStrInn inn(cmd);
	uni::LinearParser lp(inn);
	lp.GHT = false;
	lp.method_omit_spaces = false;
	lp.method_omit_comment = true;
	lp.method_string_double_quote = true;
	lp.method_string_single_quote = true;
	lp.handler_numchk = nullptr;

	uni::Dchain dc;
	lp.Parse(dc);

	for (uni::Dnode* nod = dc.Root(); nod && !syntax_error; nod = nod->next) {
		if (nod->type == tok_spaces) {
			close_word();
		}
		else if (nod->type == tok_string) {
			const char* s = nod->addr ? nod->addr : "";
			append_text(s, StrLength(s));
		}
		else if (nod->type == tok_symbol) {
			parse_symbol_span(nod->addr ? nod->addr : "");
		}
		else {
			const char* s = nod->addr ? nod->addr : "";
			if (!in_word && StrCompare(s, "2") == 0 && nod->next &&
				nod->next->type == tok_symbol && nod->next->addr &&
				nod->next->addr[0] == '>' && nod->next->addr[1] != '>') {
				begin_redir(RedirKind::Err);
				nod = nod->next;
				parse_symbol_span(nod->addr + 1);
			}
			else {
				append_text(s, StrLength(s));
			}
		}
	}
	close_word();

	if (syntax_error || pending_redir != RedirKind::None) {
		write(2, syntax_err_msg, StrLength(syntax_err_msg));
		return -1;
	}
	if (stage_count == 1 && stages[0].argc == 0) {
		return 0;
	}
	if (stages[stage_count - 1].argc == 0) {
		write(2, "sh: syntax error near unexpected token `|'\n\r", 44);
		return -1;
	}
	stages[stage_count - 1].argv[stages[stage_count - 1].argc] = nullptr;

	// Single-stage built-in commands that affect the shell process itself
	if (stage_count == 1) {
		char** stage_argv = stages[0].argv;
		unsigned stage_argc = stages[0].argc;

		if (StrCompare("exit", stage_argv[0]) == 0) {
			int retcode = (stage_argc > 1) ? atoins(stage_argv[1]) : 0;
			_exit(retcode);
			return retcode;
		}
		if (StrCompare("cd", stage_argv[0]) == 0) {
			const char* target_path = (stage_argc > 1) ? stage_argv[1] : "/";
			if (chdir(target_path) < 0) {
				write(2, "cd: failed to change directory\n\r", 32);
			}
			return 0;
		}
		if (StrCompare("+m", stage_argv[0]) == 0) {
			mode_shell = false;
			return 0;
		}
		if (StrCompare("cot+ls", stage_argv[0]) == 0 &&
			!stages[0].infile && !stages[0].outfile && !stages[0].errfile) {
			return builtin_cot_ls(stage_argc, stage_argv);
		}
	}

	// Execute 1..N pipeline stages uniformly
	int prev_read_fd = -1;
	pid_t pids[MAX_PIPE_STAGES] = {};
	unsigned spawned_count = 0;

	for (unsigned s = 0; s < stage_count; s++) {
		CommandStage& st = stages[s];
		bool is_builtin_ls = (StrCompare("cot+ls", st.argv[0]) == 0);
		char resolved_path[256];

		if (!is_builtin_ls && !resolve_path(st.argv[0], resolved_path, sizeof(resolved_path))) {
			write(2, "sh: command not found\n\r", 23);
			setenv("?", "127", 1);
			if (prev_read_fd >= 0) {
				close(prev_read_fd);
				prev_read_fd = -1;
			}
			if (stage_count == 1) return -1;
			break;
		}

		int pipefd[2] = { -1, -1 };
		if (s + 1 < stage_count) {
			if (pipe(pipefd) < 0) {
				write(2, "sh: pipe creation failed\n\r", 26);
				if (prev_read_fd >= 0) {
					close(prev_read_fd);
					prev_read_fd = -1;
				}
				break;
			}
		}

		pid_t pid = fork();
		if (pid == 0) {
			signal(SIGINT, SIG_DFL);

			if (prev_read_fd >= 0) {
				dup2(prev_read_fd, 0);
				close(prev_read_fd);
			}
			if (pipefd[0] >= 0) {
				close(pipefd[0]);
			}
			if (pipefd[1] >= 0) {
				dup2(pipefd[1], 1);
				close(pipefd[1]);
			}

			if (st.infile) {
				int fd = open(st.infile, O_RDONLY);
				if (fd < 0) {
					write(2, "sh: failed to open input file\n\r", 31);
					_exit(-1);
				}
				dup2(fd, 0);
				close(fd);
			}

			if (st.outfile) {
				int flags = O_WRONLY | O_CREAT | (st.append_out ? O_APPEND : O_TRUNC);
				int fd = open(st.outfile, flags);
				if (fd < 0) {
					write(2, "sh: failed to open output file\n\r", 32);
					_exit(-1);
				}
				dup2(fd, 1);
				close(fd);
			}

			if (st.errfile) {
				int fd = open(st.errfile, O_WRONLY | O_CREAT | O_TRUNC);
				if (fd < 0) {
					write(2, "sh: failed to open error file\n\r", 31);
					_exit(-1);
				}
				dup2(fd, 2);
				close(fd);
			}

			if (is_builtin_ls) {
				_exit(builtin_cot_ls(st.argc, st.argv));
			}

			execv(resolved_path, st.argv);
			write(2, "sh: command not found\n\r", 23);
			_exit(-1);
		}
		else if (pid > 0) {
			pids[spawned_count++] = pid;
		}
		else {
			write(2, "sh: fork failed\n\r", 17);
			if (pipefd[0] >= 0) close(pipefd[0]);
			if (pipefd[1] >= 0) close(pipefd[1]);
			break;
		}

		if (prev_read_fd >= 0) {
			close(prev_read_fd);
		}
		if (pipefd[1] >= 0) {
			close(pipefd[1]);
		}
		prev_read_fd = pipefd[0];
	}

	if (prev_read_fd >= 0) {
		close(prev_read_fd);
	}

	int last_status = 0;
	for (unsigned i = 0; i < spawned_count; i++) {
		int status = 0;
		pid_t child_pid = wait(&status);
		if (child_pid > 0) {
			if (stage_count == 1 && status) {
				outsfmt("sh: process %d exited with code %d\n\r", child_pid, status);
			}
			if (child_pid == pids[spawned_count - 1]) {
				last_status = status;
			}
		}
	}

	if (spawned_count > 0) {
		char status_str[16];
		outsfmtbuf(status_str, "%d", last_status);
		setenv("?", status_str, 1);
	}
	return 0;
}
#else

void print_prompt() {
	printf("> ");
}

#endif

