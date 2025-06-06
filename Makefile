TARGET = cdstore
SRC = cdstore.c

CC = gcc

CFLAGS = -O2 -Wextra -std=c11

LDLIBS = -lsqlite3

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
