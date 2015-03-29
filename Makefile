CC = gcc
CFLAGS = -o breakout
LDFLAGS = -lncurses -lm

OBJS = cursed-breakout.o
a.out: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS)

clean: 
	rm $(OBJS) breakout

debug: CFLAGS+=-DDEBUG=1
debug: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS)

love:
	clear;echo "not war"