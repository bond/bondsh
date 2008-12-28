#define LINE_BUFFER_MAX				4096
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