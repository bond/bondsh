#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include "bondsh.h"
#include <errno.h>
#include <limits.h>
#include <err.h>
#include <readline/readline.h>


static char *line_read = (char *)NULL;
static char *std_prompt = "%u %B%~%b> ";
/* launch off forks
*/
void execute_chain(bsh_command_chain_t *chain, char *envp[], const char *path) {
	bsh_command_chain_t *cmd;
	int i;
	for(cmd = chain;cmd != NULL; cmd = cmd->next) {
		if(cmd->next  && cmd->next->op == PIPE_STDIN) {
			/* The next entry in the chain, wants something into it's input.
				 Create the pipe in the next object, so this entry can access it via 'next'.
			*/
			if (pipe(cmd->next->fds) == -1) errx(errno, "Unable to create pipe(%d,%d): %s", cmd->next->fds[0], cmd->next->fds[1], strerror(errno)); // if there is a next cmd
//			printf("created pipe, fds are: %d and %d\n", cmd->next->fds[0], cmd->next->fds[1]);
		}
		if((cmd->pid = fork()) == -1) errx(errno, "Unable to fork()!");

		if(cmd->pid == 0) {

			/* there is something connected to pipe fds[1] */
			if(cmd->op == PIPE_STDIN) {
			//	printf("READ STDIN: %s\n", cmd->args[0]);
				dup2(cmd->fds[0], STDIN_FILENO); 
        close(cmd->fds[0]);
  			close(cmd->fds[1]);
			}

			/* connect something to next->pipe->fds[1] */
			if(cmd->next && cmd->next->op == PIPE_STDIN) {
			//	printf("WRITE STDOUT: %s\n", cmd->args[0]);
				close(cmd->next->fds[0]);
			 	dup2(cmd->next->fds[1], STDOUT_FILENO);
				close(cmd->next->fds[1]);
			}

			/* run the program, and print a friendly message if program not in path */
			if(execvP(cmd->command, path, cmd->args) < 0){
				switch(errno) {
					case ENOENT:
						errx(errno, "Could not find program: '%s' in PATH", cmd->command);

					default:
						errx(errno, "Failed to execvP(""%s"", argv, envp), errno is %d", cmd->command, errno);
				}
			}
			// this should not happen
			errx(-1, "unhandled exception");
		} else if(cmd->pid > 0 && cmd->op == PIPE_STDIN) { // parent proc has a previous
			close(cmd->fds[0]);
			close(cmd->fds[1]);
		}
	}
	for(cmd = chain;cmd != NULL; cmd = cmd->next) {
		/* To start off with, we just simplify the WAIT-code, and see
		if it's really needed */
		waitpid(cmd->pid, &i, 0);
		// if(i != 0) {
		// 	printf("ups, %s[%d] exited with status %d\n", cmd->command, cmd->pid, i);
		// }
	}
}

char *rl_gets()
{
	char *prompt = NULL;
	if(line_read) {
		free(line_read);
		line_read = (char *)NULL;
	}
	/* add path and grab the line */
	prompt = prompt_expand(std_prompt);
	line_read = readline(prompt);

	/* add to history if it contains anything */
	if(line_read && *line_read) 
			add_history(line_read);

	return line_read;
}

int main (int argc, char const *argv[], char *envp[])
{
	char line[LINE_BUFFER_MAX];
	bsh_command_chain_t *chain, *current;
	
	/* pick out important info from ENV */
	int i = 0;

	char *path = NULL;
	char *p = NULL;
	for(i = 0; envp[i] != NULL; i++) {
		/* PATH= */
		if((p = strchr(envp[i], '=')) == NULL) continue;
		if(strlen(envp[i]) > 5 && strncmp(envp[i], "PATH=", 5) == 0) {
			path = strdup(envp[i]+5);
			assert(path);
		}
	}

	for(;;) {
		/* MAIN LOOP */
		rl_gets();
		if(line_read == NULL) return;
		//printf("got new line from readline: %s\n", line_read);
		chain = build_chain_from_str(line_read);
		if(chain == NULL) continue;
    //chain_print(chain);
		//return 0;

		execute_chain(chain, (char **)envp, path);

		chain_free(chain);
		free(chain);
 	}  
	
	return 0;
}
