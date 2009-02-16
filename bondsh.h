#define LINE_BUFFER_MAX				4096
#define HISTORY_MAX					1024
#define PROGRAM_NAME				"bondsh"

typedef void (*sighandler_t)(int);


// INPUT CONTEXT TYPES
#define CONTEXT_NOCONTEXT			0x0000
#define	CONTEXT_EXECUTE				0x0001
#define CONTEXT_LUA					0x0002
#define	CONTEXT_SIGNAL				0x0003

#define	CHAIN_WANT_COMMAND			0x0004
#define CHAIN_WANT_ARGUMENT			0x0005
#define CHAIN_WANT_LAST_COMMAND		0x0006
#define CHAIN_WANT_LAST_ARGUMENT	0x0007
#define CHAIN_WANT_PROCESS_PIPE		0x0008
#define	CHAIN_BUFFER_SKIP			0x0009
#define CHAIN_WANT_COMPLETION		0x0010

#define CHAR_TAB					 9
#define CHAR_SLASH					 47
#define CHAR_NEWLINE				10
#define	CHAR_SPACE					32
#define CHAR_PIPE					124

// COMMAND-CHAIN DATATYPES


typedef struct {
	char *command;
	char *args[LINE_BUFFER_MAX / 2];
	int num_args;
	void *next;
	} bsh_command_chain_t;
	
	typedef struct {
		bsh_command_chain_t *chain;
		void *next;
	} bsh_history_chain_t;
	
	int chain_get_state(bsh_command_chain_t*, char*, char);
	void chain_set_argument(bsh_command_chain_t*, char*);
	void chain_set_command(bsh_command_chain_t*, char*);
	int determine_input_context(bsh_command_chain_t*);
	void chain_init(bsh_command_chain_t*);
	void chain_free(bsh_command_chain_t*);
	void print_prompt();
	
	// #history
	void history_init(bsh_history_chain_t*);
	void history_attach(bsh_history_chain_t*, bsh_command_chain_t*);
	