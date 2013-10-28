# Arkurses!

all : arkurses.c
	@cc arkurses.c -lpanel -lncurses -o arkurses -Os -march=native

run : arkurses.c arkurses
	@./arkurses

commit : .git
	@git commit -a && git push

clean :
	@rm arkurses *~
