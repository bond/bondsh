#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "bondsh.h"
#include <errno.h>
#include <assert.h>


int determine_input_context(bsh_command_chain_t *chain) {

	int c,i; // c=char,i=index of char
	char *p = NULL;
//	int pipes = 0; //number of pipes in this line
	
	char input_buf[LINE_BUFFER_MAX];
	bzero(&input_buf, sizeof(input_buf));
	
	/* Get input */	
	for(i = 0; (c = getchar()) && (i - 1) < LINE_BUFFER_MAX;) {
		if( feof(stdin) != 0 ) { exit(0); /* EOF */ }

		switch(chain_get_state(chain, (char *)&input_buf, c)) {
			case CHAIN_WANT_COMMAND:
//				printf("want command '%s'\n", input_buf);
				p = (char *)&(input_buf[i]);
				chain_set_command(chain, (char *)&input_buf);
				break;
			case CHAIN_WANT_ARGUMENT:
//				printf("want argument '%s'\n", p);
				chain_set_argument(chain, p);
				p = (char *)&(input_buf[i]);				
				break;
				
			case CHAIN_WANT_LAST_COMMAND:
//				printf("want last command\n");
				chain_set_command(chain, (char *)&input_buf);
				goto CHAIN_END;

			case CHAIN_WANT_LAST_ARGUMENT:
//				printf("want last argument '%s'\n", p);
				if(strlen(p) > 0) {
					chain_set_argument(chain, p);
				}
				goto CHAIN_END;
				
			case CHAIN_WANT_PROCESS_PIPE:
				if(chain->command == NULL) { chain_set_command(chain, (char *)&input_buf); }
				else { chain_set_argument(chain, p); }

				return CHAIN_WANT_PROCESS_PIPE;
				break;
				
			case CHAIN_BUFFER_SKIP:
				break;
				
			case CHAIN_WANT_COMPLETION:
			printf("got tab!\n");
				break;

			default:
				strncat(input_buf, (char *)&c, 1);
				i++;
				break;
		}
		// if(c == 32) {
		// 	chain_set_command(&cc, input_buf);
		// }
		// if(c == 124) pipe_seen++;
		// 
		// strncat(input_buf, (char *)&c, 1);
	}
	return CONTEXT_NOCONTEXT;
	
CHAIN_END:
	/* Determine stuff */
//	printf("\n");
	
	/* set context data and return */
	return CONTEXT_EXECUTE;
}