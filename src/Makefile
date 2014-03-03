SHELL = /bin/sh
.SUFFIXES: .c .o
CC = gcc 
CFLAGS = -O -g -Wall 
INCLUDES = -I../include -I../lib
LDLIBS = -L../lib -lutil -L/usr/lib -lm -lgsl -lblas -llapack 

objects = main.o bj_sims.o bj_strat.o hands.o print_chart.o 
executable = BlackjackStrategy

$(executable) : $(objects)
	$(CC) $(CFLAGS) -o $(executable) $(objects) $(INCLUDES) $(LDLIBS)

.c.o : 
	$(CC) $(CFLAGS) -c $< $(INCLUDES)

.PHONY: clean
clean :
	-rm -f $(executable) $(objects)

