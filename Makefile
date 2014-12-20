CC = gcc
CFLAGS = -o breakout
LDFLAGS = -lncurses

OBJS = cursed-breakout.o
a.out: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS)

clean: 
	rm $(OBJS) breakout

debug: CFLAGS+=-DDEBUG=1
debug: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS)
