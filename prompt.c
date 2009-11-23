#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "bondsh.h"

char *system_getuser() {
	char *ret = NULL;
	if(asprintf(&ret, "%d", getuid()) == -1) errx(-1, "Unable to allocate memory!");
	return ret;
}

char *system_getpwd() {
	char *ret = NULL;
	ret = getwd(ret);
	if(ret== NULL) errx(-1, "getcwd() failed!");
	return ret;
}

char *prompt_expand(char *fmt) {
	char buf[64];
	bzero(buf, sizeof(buf));
	int i;
	char *tmp = NULL;
	for(i = 0; fmt[i] != '\0'; fmt++) {
		if(fmt[i] == CHAR_PERCENT)
			if(fmt[i + 1] != '\0'){
				i++;
				switch(fmt[i]) {
					case 'u': // username
						tmp = system_getuser();
						strncat(buf, tmp, strlen(tmp));
						free(tmp);
						continue;

					case 'b':
					case 'B': // turn bold on or off
					tmp = (fmt[i] == 'B' ? "\033[1;1m" : "\033[1;m");
					strncat(buf, tmp, strlen(tmp)); // XTERM
					continue;
					case '~':
						tmp = system_getpwd();
						strncat(buf, tmp, strlen(tmp)); // XTERM
					continue;

					case '%':
					continue;

					default:
					i--;
		 		}
		 }
		 strncat(buf, &(fmt[i]), 1);
	}

	char *l = strdup(buf); 
	assert(l);
	return l; 
}