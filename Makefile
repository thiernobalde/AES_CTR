SHELL =/bin/bash
CC = gcc
RM = rm -f
CFLAGS = -W -Wall
EXECS = aes
OBJS =  util.o aes.o modeCompteur.o

.c.o :
	$(CC) $(CFLAGS) -c $*.c


all:: $(EXECS)



$(EXECS) : $(OBJS)
	$(CC) -o $@ $^

clean ::
	$(RM) *.o $(EXECS)
