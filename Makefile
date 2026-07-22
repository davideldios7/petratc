CC = gcc
CFLAGS = -Wall -Wextra -O2 -pipe 
LDLIBS = -lncurses -ltinfo
TARGET = petratc
SRC = petratc.c rat.c $(wildcard games/*.c)
PREFIX = /usr/local

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDLIBS)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)
