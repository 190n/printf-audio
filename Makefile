CC = clang
CFLAGS = -std=c2x -fsanitize=undefined,address -Wall -Wextra -pedantic -Wno-format -Werror -g3 -O2
LFLAGS = -fsanitize=undefined,address -lsoundio
OBJS = garbage.o

.PHONY: all clean format

all: garbage

garbage: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f garbage *.o

format:
	clang-format -i -style=file *.[ch]
