CFLAGS = -g -Werror
LDFLAGS = -lncurses -lreadline

INPUT_FILES = *.c

all: bondsh

bondsh: $(INPUT_FILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	$(RM) bondsh
