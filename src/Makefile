SHELL = /bin/sh
.SUFFIXES: .c .o
CC = gcc 
CFLAGS = -O -g -Wall
LDLIBS = -L./util -lutil -L/usr/lib -lm -lgsl -lblas -llapack 

objects = main.o bj_sims.o bj_strat.o hands.o print_chart.o 
executable = BlackjackStrategy

$(executable) : $(objects)
	$(CC) $(CFLAGS) -o $(executable) $(objects) $(LDLIBS)

main.o : bj_strat.h hands.h print_chart.h 
bj_sims.o : bj_strat.h hands.h 
bj_strat.o : hands.h
hands.o : bj_strat.h
print_chart.o : bj_sims.h bj_strat.h hands.h 

.PHONY: clean
clean :
	-rm -f $(executable) $(objects)

