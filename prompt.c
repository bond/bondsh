#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <histedit.h>

char *strp_left_prompt() {
	char *l = "bsh > ";
	return l;
}
void print_prompt() {
	printf("%s", strp_left_prompt());
}
