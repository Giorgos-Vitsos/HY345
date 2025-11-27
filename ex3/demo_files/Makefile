CC = gcc
CFLAGS = -Wall -std=c99
TARGET = demo_program
SOURCES = demo_program.c
HEADERS = proc_info_user.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET) *.o