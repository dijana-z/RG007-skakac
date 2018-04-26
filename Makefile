CC 			= gcc
CCFLAGS 	= -Wall -Wextra -ggdb -std=c99
BIN 		= build
OBJFILES 	= $(BIN)/callbacks.o $(BIN)/functions.o
LDFLAGS 	= -lglut -lGLU -lGL -lm
TARGET 		= $(BIN)/jumping_cube

all: $(BIN) $(TARGET)

$(TARGET): $(BIN)/main.o $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN)/main.o: main.c
	$(CC) $(CCFLAGS) -c -o $@ $<

$(BIN)/%.o: %.c %.h
	$(CC) $(CCFLAGS) -c -o $@ $<

$(BIN):
	@test ! -d $@ && mkdir $@

.PHONY: clean zip

clean:
	rm -rf $(BIN)

zip:
	zip -r $(TARGET).zip ./
