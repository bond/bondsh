#include "bondsh.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

void chain_init(bsh_command_chain_t *cc) {
	cc->command = NULL;
	bzero(cc->args, sizeof(cc->args));
	cc->num_args = 0;
	cc->next = NULL;
	cc->args[cc->num_args++] = strdup(PROGRAM_NAME);
	assert(cc->args[0] && cc->num_args);
}
void chain_free(bsh_command_chain_t *cc) {
//	printf("freeing command: %s\n", cc->command);
	int i;
	if(cc->command != NULL) free(&(cc->command));
	if(cc->next != NULL) free(&(cc->next));
	for(i = 0; cc->args[i] != NULL; i++) {
		free(cc->args[i]);
	}
	//free(cc);
}
void chain_set_command(bsh_command_chain_t *cc, char *command) {
	cc->command = strdup(command);
	assert(cc->command);
}
void chain_set_argument(bsh_command_chain_t *cc, char *argument) {
	cc->args[cc->num_args] = strdup(argument);
	assert(cc->args[cc->num_args]);
	cc->num_args++;
}
int chain_get_state(bsh_command_chain_t *cc, char *buf, char c) {
	switch(c) {
		case CHAR_NEWLINE:
			/* we don't care about preceding newlines */
			if(strlen(buf) == 0) { print_prompt(); return CHAIN_BUFFER_SKIP;}
			if(cc->command == NULL) return CHAIN_WANT_LAST_COMMAND;
			return CHAIN_WANT_LAST_ARGUMENT;
		case CHAR_SPACE:
			/* we don't care about preceding spaces */
			if(strlen(buf) == 0) return CHAIN_BUFFER_SKIP;
			if(cc->command == NULL) return CHAIN_WANT_COMMAND;
			return CHAIN_WANT_ARGUMENT;
		case CHAR_PIPE:
			return CHAIN_WANT_PROCESS_PIPE;
		default:
			return CONTEXT_NOCONTEXT;			
	}

	/* default, just adds characters to the line_buf */
	return CONTEXT_NOCONTEXT;
}
