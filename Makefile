TARGET = jumping_cube
CC = gcc
CFLAGS = -Wall -Wextra
OBJFILES = main.o callbacks.o
LDLIBS = -lglut -lGLU -lGL -lm

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean

clean:
	rm -f $(TARGET) *.o
