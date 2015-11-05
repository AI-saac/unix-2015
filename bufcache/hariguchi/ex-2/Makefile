# Makefile
PROGRAM = bufcache
OBJS = main.o
CC = gcc 

$(PROGRAM) : $(OBJS)
	$(CC) -o $(PROGRAM) $^
main.o : main.c
	$(CC) -c $<
clean : 
	$(RM)  *.o *~
