CC = GCC
FLAGS = -Wall -c

#compilation

all:
	rm -rf obj/
	mkdir obj

	$(CC) $(FLAGS) main.c -o obj/main.o
	$(CC) $(FLAGS) uiTools.c -o obj/uiTools.o

	$(CC) obj/* -o crash_n_bob