#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "bondsh.h"
#include <errno.h>
#include <assert.h>

bsh_command_chain_t *build_chain_from_str(char *line) {
	bsh_command_chain_t *chain = chain_init();
	bsh_command_chain_t *current = chain;
	bsh_command_chain_t *b = current;
	char *linep = line;

MORE_INPUT:
	
	switch(parser_read(current, &line)) {
		case CHAIN_WANT_PROCESS_PIPE:
			//printf("CHAIN_WANT_PROCESS_PIPE:\n");
			current->next = chain_init();
			current = current->next;
			current->op = PIPE_STDIN;
			goto MORE_INPUT;
			break;

		case CHAIN_DONE:
			//printf("CHAIN_DONE:\n");
			return chain;

		case CONTEXT_NOCONTEXT:
			/* cleanup elements which have been given invalid arguments, like pipes without args */
			for(current = chain; current != NULL; current = current->next) {
				if(current->command == NULL)
					if(current == chain) return NULL;
					else{
						chain_free(current);
						free(current);
						b->next = NULL;
					}

				b = current;
			}
			return chain;

		default:
			errx(-1, "error!");

	}

	return chain;
}

int parser_read(bsh_command_chain_t *chain, char **line) {
	char buf[LINE_BUFFER_MAX];
	char *p = *line;
	int i;

	bzero(&buf, sizeof(buf));

	for(i=0;(p=(*line)+i)[0] != '\0'; i++) {
		switch(p[0]) {
			case CHAR_SPACE:
				if(strlen(buf) > 0) {
					if(chain->command == NULL) chain_set_command(chain, (char *)&buf);
					else { chain_set_argument(chain, (char *)&buf); }
					bzero(buf, sizeof(buf));
				}
			break;
			case CHAR_PIPE:
			if(chain->command != NULL) {
				*line = ++p; // adjust line, to the current parser location
				return CHAIN_WANT_PROCESS_PIPE;
			} else if(strlen(buf) > 0) {
					*line = ++p; // adjust line ..
					chain_set_command(chain, (char *)&buf);
					return CHAIN_WANT_PROCESS_PIPE;
			}
			break;
			default:
			strncat(buf, p, 1);
		}
	}

	if(strlen(buf) > 0)
		if(chain->command == NULL) chain_set_command(chain, (char *)&buf);
		else chain_set_argument(chain, (char *)&buf);

			if(strlen(buf) == 0) return CONTEXT_NOCONTEXT;
	return CHAIN_DONE;
}
