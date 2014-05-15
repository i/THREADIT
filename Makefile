FLAGS= -Wall -Werror
EXTRA= -Wextra
DEBUG= -g

all:
	gcc $(FLAGS) mypthread.c mymutex.c user.c -o part2 -g

debug: all

clean:
	rm part2
