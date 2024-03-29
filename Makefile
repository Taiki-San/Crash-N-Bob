CC = gcc
FLAGS = -Wall -c --std=c99

TARGET = CrashNBob

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ obj_*.o

%.o: %.c
	$(CC) $(FLAGS) $^ -o obj_$@

clean:
	rm -r $(TARGET) obj_*