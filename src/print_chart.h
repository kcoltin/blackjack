/*
 * print_chart.h
 * Kevin Coltin  
 *
 * Contains functions for printing a chart of blackjack strategy to Latex. 
 */

#ifndef PRINT_CHART_H 
#define PRINT_CHART_H 

#include <stdio.h> 
#include "bj_sims.h" 
#include "bj_strat.h" 

void printChart (Strategy **chart, const char *filename, int showWinPct, 
            int MAKE_SIMPLE_CHART); 
void printHand (int i, Strategy **chart, int showWinPct,  FILE *file);
char * actionSymbol (int); 
void printSimsChart (HandSim **simsChart, Strategy **chart, 
              const char *filename, int nsims); 
void printHandSims (int i, HandSim **simsChart, Strategy **chart, FILE *file,
              int nsims);

#endif 
