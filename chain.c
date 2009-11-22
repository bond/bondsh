#include "bondsh.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

bsh_command_chain_t *chain_init() {
	bsh_command_chain_t *cc = malloc(sizeof(bsh_command_chain_t));
	assert(cc);
 	cc->command = NULL;	
	bzero(cc->args, sizeof(cc->args));
	cc->num_args = 0;
	cc->op = 0;
	cc->next = NULL;
 	cc->args[cc->num_args++] = strdup(PROGRAM_NAME);	
 	assert(cc->args[0] && cc->num_args);
	return cc;
}
void chain_free(bsh_command_chain_t *cc) {
	int i;
	if(cc->next) { chain_free(cc->next); free(cc->next); }
	if(cc->command) free(cc->command);
	for(i = 0; cc->args[i] != NULL; i++) free(cc->args[i]);
}
void chain_set_command(bsh_command_chain_t *cc, char *command) {
	cc->command = strdup(command);
	cc->args[0] = strdup(command);
	assert(cc->command);
	assert(cc->args[0]);
}

void chain_set_argument(bsh_command_chain_t *cc, char *argument) {
	if(strlen(argument) > 0) {
		cc->args[cc->num_args] = strdup(argument);
		assert(cc->args[cc->num_args]);
		cc->num_args++;
	}
}

void chain_print(bsh_command_chain_t *cc) {
	static int x = 0;
	int i;
	printf("chain: %d\n", x);
	printf("chain->command: '%s'\n", cc->command);
	printf("chain->args:\n");
	for(i = 0; cc->args[i] != NULL; i++) printf("\t[%d]: '%s'\n", i, cc->args[i]);
	printf("\n");
	if(cc->next) {x++; chain_print(cc->next); x--;}
}
