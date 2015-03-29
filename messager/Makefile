#Use the gcc compiler
CC = gcc
CFLAGS =
DEPS =
LDFLAGS =
#Objects created by makefile
OBJS = server client threaded-server

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all


server: server-msg.c
	$(CC) $(CFLAGS) -o $@ $^

client: client-msg.c
	$(CC) $(CFLAGS) -o $@ $^

threaded-server: threaded-server-msg.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
