CC         = clang
OPT        =
SANITIZE   = -fsanitize=undefined,memory -fsanitize-memory-track-origins -fsanitize-trap
LOOSEFLAGS = -std=c2x -g3 -Ivendor/miniaudio $(OPT) $(SANITIZE)
CFLAGS     = -Wall -Wextra -pedantic -Wno-format-invalid-specifier -Wconversion -Wsign-conversion -Werror -Wno-format $(LOOSEFLAGS)
LFLAGS     = $(SANITIZE) -lm
OBJS       = main.o stb.o miniaudio.o
HEADERS    = $(wildcard *.h)

.PHONY: all clean format

all: printf-audio

printf-audio: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $<

stb.o: stb.c $(HEADERS)
	$(CC) $(LOOSEFLAGS) -Wno-constant-logical-operand -c $<

miniaudio.o: miniaudio.c $(HEADERS)
	$(CC) $(LOOSEFLAGS) -c $<

clean:
	rm -f garbage *.o

format:
	clang-format -i -style=file *.[ch]
