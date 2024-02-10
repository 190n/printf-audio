CC = clang
LOOSEFLAGS = -std=c2x -fsanitize=undefined,address -g3 -Ivendor/miniaudio -O2
CFLAGS = -Wall -Wextra -pedantic -Wno-format-invalid-specifier -Wconversion -Wsign-conversion -Werror -Wno-format $(LOOSEFLAGS)
LFLAGS = -fsanitize=undefined,address -lm
OBJS = main.o stb.o miniaudio.o
HEADERS = $(wildcard *.h)

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
