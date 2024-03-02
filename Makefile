
CC = gcc
CFLAGS = -std=c99 -g -pedantic -Wall
CPPFLAGS = -DDEBUG -D_XOPEN_SOURCE=700L


prompt: prompt.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

install: prompt
	cp prompt /usr/local/bin

clean:
	rm prompt
