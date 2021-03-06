#include <sys/types.h>


#define LINE_BUFFER_MAX			4096
#define HISTORY_MAX					1024
#define PROGRAM_NAME				"bondsh"

typedef void (*sighandler_t)(int);


// INPUT CONTEXT TYPES
#define CONTEXT_NOCONTEXT				0x0000
#define CONTEXT_EXECUTE				        0x0001
#define CONTEXT_LUA					0x0002
#define CONTEXT_SIGNAL					0x0004

#define CHAIN_WANT_COMMAND				0x0001
#define CHAIN_WANT_ARGUMENT				0x0002
#define CHAIN_WANT_LAST_COMMAND			        0x0004
#define CHAIN_WANT_LAST_ARGUMENT	                0x0008
#define CHAIN_WANT_PROCESS_PIPE			        0x0016
#define CHAIN_BUFFER_SKIP				0x0032
#define CHAIN_WANT_COMPLETION				0x0064
#define CHAIN_WANT_FILE_REDIRECTION	0x0128
#define CHAIN_DONE									0x0256

#define PIPE_STDIN					0x0100

#define CHAR_LT						60
#define CHAR_GT						62

#define CHAR_TAB					9
#define CHAR_SLASH					47
#define CHAR_NEWLINE				        10
#define CHAR_SPACE					32
#define CHAR_PIPE					124
#define	CHAR_PERCENT			37

// COMMAND-CHAIN DATATYPES


typedef struct _bshcc {
	char *command;
	char *args[LINE_BUFFER_MAX / 2];
	int num_args;
	pid_t pid;
	int fds[2];
	int op;
	struct _bshcc *next;
	} bsh_command_chain_t;
	
	typedef struct {
		bsh_command_chain_t *chain;
		void *next;
	} bsh_history_chain_t;
	
	void chain_set_argument(bsh_command_chain_t*, char*);
	void chain_set_command(bsh_command_chain_t*, char*);
	int parser_read(bsh_command_chain_t*, char**);
	int determine_input_context(bsh_command_chain_t*);
	bsh_command_chain_t *build_chain_from_str(char *line);
	bsh_command_chain_t *chain_init();
	void chain_free(bsh_command_chain_t*);
	void chain_print(bsh_command_chain_t*);
	char *prompt_expand(char *fmt);
	
	// #history
	void history_init(bsh_history_chain_t*);
	void history_attach(bsh_history_chain_t*, bsh_command_chain_t*);
