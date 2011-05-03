
CC = gcc
CFLAGS = -c
SOURCES = main.c
OBJECTS = $(SOURCES:.c=.o)
EXE = wsbridge

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o
