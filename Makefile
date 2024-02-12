CC         = clang
OPT        =
SANITIZE   = -fsanitize=undefined,address
LOOSEFLAGS = -std=c2x -g3 -Ivendor/miniaudio $(OPT) $(SANITIZE)
CFLAGS     = -Wall -Wextra -pedantic -Wno-format-invalid-specifier -Wconversion -Wsign-conversion -Werror -Wno-format $(LOOSEFLAGS)
LFLAGS     = $(SANITIZE) -lm
OBJS       = main.o stb.o miniaudio.o

.PHONY: all clean format

all: printf-audio

printf-audio: $(OBJS) Makefile.deps
	$(CC) $(LFLAGS) -o $@ $(OBJS)

Makefile.deps: $(wildcard *.c)
	$(CC) $(CFLAGS) -MM $^ > $@

include Makefile.deps

%.o: %.c
	$(CC) $(CFLAGS) -c $<

stb.o: stb.c
	$(CC) $(LOOSEFLAGS) -Wno-constant-logical-operand -c $<

miniaudio.o: miniaudio.c
	$(CC) $(LOOSEFLAGS) -fno-sanitize=memory -c $<

clean:
	rm -f printf-audio *.o Makefile.deps

format:
	clang-format -i -style=file *.[ch]
