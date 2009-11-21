CC = gcc
CFLAGS = -g -Werror
LDFLAGS =

FILES = *.c
# cc -Wall -o bondsh *.c

all: bondsh

bondsh: $(FILES)
	cc $(CFLAGS) -o $@ $^

clean:
	$(RM) bondsh
