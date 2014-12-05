# Arkurses!
objs = main.o arkurses.o
exe = Arkurses
CC = gcc
CFLAGS = -g -O2 -march=native -lcurses -lpanel

all : $(objs)
	$(CC) $(CFLAGS) $(objs) -o $(exe)

run :
	@./$(exe)

clean :
	$(RM) $(exe) $(objs) *~
