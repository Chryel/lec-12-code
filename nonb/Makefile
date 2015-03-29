#Use the gcc compiler
CC = gcc
CFLAGS =
DEPS =
LDFLAGS =
#Objects created by makefile
OBJS = nonb-server

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

nonb-server: nonb-server-msg.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
