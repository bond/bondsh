#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <histedit.h>

char *strp_left_prompt() {
	char *l = "b > ";
	return l;
}
void print_prompt() {
	printf("%s", strp_left_prompt());
}
char *get_prompt(EditLine *e) {
	char *p = strdup("bsh> ");
	assert(p);
	return p;
}