TARGET = jumping_cube
CC = gcc
CFLAGS = -Wall -Wextra
OBJFILES = main.o callbacks.o functions.o
LDLIBS = -lglut -lGLU -lGL -lm

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean zip

clean:
	rm -f $(TARGET) *.o

zip:
	zip -r $(TARGET).zip ./
