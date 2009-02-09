#include "bondsh.h"
#include <string.h>

void history_init(bsh_history_chain_t *hist) {
	hist->chain = NULL;
	hist->next = NULL;
}

void history_attach(bsh_history_chain_t *hist, bsh_command_chain_t *chain) {
	
}