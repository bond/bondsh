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
#include <editline/readline.h>


void sighandle_line_clear(int sig) {
	fflush(stdout);
}

/* 1 - launch off forks
*/
void execute_chain2(bsh_command_chain_t *chain, char *envp[], const char *path) {
	bsh_command_chain_t *cmd;
	int i, pids_left;
	for(cmd = chain;cmd != NULL; cmd = cmd->next) {
		if(cmd->next  && cmd->next->op == PIPE_STDIN) {
			// FIXME: check return value for pipe();
			pipe(cmd->next->fds); // if there is a next cmd
//			printf("created pipe, fds are: %d and %d\n", cmd->next->fds[0], cmd->next->fds[1]);
		}
		// FIXME: check return value for fork();
			cmd->pid = fork();
			if(cmd->pid == 0) {

			/* there is a previous cmd */
			if(cmd->op == PIPE_STDIN) {
			//	printf("READ STDIN: %s\n", cmd->args[0]);
				dup2(cmd->fds[0], STDIN_FILENO); 
        close(cmd->fds[0]);
  			close(cmd->fds[1]);
			}

			/* there is a next OPERATION */
			if(cmd->next && cmd->next->op == PIPE_STDIN) {
			//	printf("WRITE STDOUT: %s\n", cmd->args[0]);
				close(cmd->next->fds[0]);
			 	dup2(cmd->next->fds[1], STDOUT_FILENO);
				close(cmd->next->fds[1]);
			}


			printf("executing: ");
			for(i = 0; cmd->args[i]; i++) printf(" %s", cmd->args[i]);
			printf("\n");

			i = execvP(cmd->command, path, cmd->args);
			if(i < 0){
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
WAIT:
	i = 0;
	pids_left = 0;
	for(cmd = chain;cmd != NULL; cmd = cmd->next) {
		i++;
		if(!cmd->pid) continue;
		// TODO: check return value for waitpid
		if( waitpid(cmd->pid, &i, WNOHANG) > 0) {   
				cmd->pid = 0;
				printf("%s exited with status %d\n", cmd->args[0], i);    
		} else {
			printf("waiting for: %s %s\n", cmd->args[0], cmd->args[1]);
			pids_left++;
		}
	}
	if(pids_left) {
		printf("%d pids waiting\n", pids_left);
		sleep(1);
		goto WAIT;
	}
}

int main (int argc, char const *argv[], char *envp[])
{
	setbuf(stdin, NULL);
	/* define signal handlers */
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, sighandle_line_clear);
	
	/* code */
	char line[LINE_BUFFER_MAX];
	bsh_history_chain_t hist;
	history_init(&hist);
	
	/* pick out important info from ENV */
	int i = 0;
	char *path = NULL;
	for(i = 0; envp[i] != NULL; i++) {
		/* PATH= */
		if(strncmp(envp[i], "PATH=", 5) == 0) {
			path = strdup(envp[i]+5);
			assert(path);
		}
	}

	for(;;) {
		bzero(line, sizeof(line));
		bsh_command_chain_t *chain = chain_init();
		bsh_command_chain_t *current = chain;
		
		print_prompt();
		
MORE_INPUT:
		switch(determine_input_context(current)) {
			case CHAIN_WANT_PROCESS_PIPE:
				current->next = chain_init();
				current = current->next;
				current->op = PIPE_STDIN;
				goto MORE_INPUT;

			case CONTEXT_EXECUTE:
			printf("executing chain!\n");
				execute_chain2(chain, (char **)envp, path);
				fflush(stdout);
				break;

			case CONTEXT_NOCONTEXT:
				printf("NOCONTEXT caught\n");
				break;
				
			default:
				printf("error!\n");
				exit(-1);
				break;
		}
		chain_free(chain);
		// history_attach(&hist, &chain);
	}
	
	return 0;
}