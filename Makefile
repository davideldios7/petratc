CC = gcc
CFLAGS = -Wall -Wextra -O2 -pipe 
LDLIBS = -lncurses
TARGET = petratc
SRC = petratc.c rat.c $(wildcard games/*.c)
OBJ = $(SRC:%.c=obj/%.o)
PREFIX = /usr/local

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDLIBS)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

obj/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)
