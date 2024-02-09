CC = clang
LOOSEFLAGS = -std=c2x -fsanitize=undefined,address -g3 -O2 -Ivendor/miniaudio
CFLAGS = -Wall -Wextra -pedantic -Wno-format-invalid-specifier -Wconversion -Wsign-conversion -Werror -Wno-format $(LOOSEFLAGS)
LFLAGS = -fsanitize=undefined,address
OBJS = garbage.o stb.o miniaudio.o bruh.o
HEADERS = $(wildcard *.h)

.PHONY: all clean format

all: garbage

garbage: $(OBJS)
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
