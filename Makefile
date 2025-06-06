TARGET = cdstore
SRC = cdstore.c

CC = gcc

CFLAGS = -O2 -Wextra -std=c11

LDLIBS = -lsqlite3

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDLIBS)

install: all
	@echo "Installing $(TARGET) to $(BINDIR)"
	install -m 755 $(TARGET) $(BINDIR)


clean:
	rm -f $(TARGET)

.PHONY: all clean
