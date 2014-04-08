#include "print_chart.h"
#include <math.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include "error.h"
#include "bj_sims.h" 
#include "bj_strat.h" 
#include "hands.h" 


//------------------------------------------------------------------------------
// Prints a chart of strategies to a .tex file. 
//------------------------------------------------------------------------------
void printChart (Strategy **chart, const char *filename, int showWinPct, 
            int MAKE_SIMPLE_CHART)
{
  FILE *file = NULL; 
  int i, j; 
  
  file = fopen(filename, "w"); 
  if (file == NULL) throwErr("File could not be opened.", "printChart");
  
  fprintf(file, "\\documentclass{article}\n\n"); 
  fprintf(file, "\\usepackage{amsmath, amssymb}\n"); 
  fprintf(file, "\\pagenumbering{gobble}\n\n"); 
  fprintf(file, "\\addtolength{\\oddsidemargin}{-.5in}\n"); 
  fprintf(file, "\\addtolength{\\evensidemargin}{-.5in}\n"); 
  fprintf(file, "\\addtolength{\\textwidth}{1in}\n"); 
  fprintf(file, "\\addtolength{\\topmargin}{-1.5in}\n"); 
  fprintf(file, "\\addtolength{\\textheight}{2.3in}\n\n"); 

  fprintf(file, "\\begin{document}\n\n\\begin{center}\n\\begin{large}\n"); 
  fprintf(file, "Blackjack Strategy\n\\end{large}\n\\end{center}\n\n"); 
  
  fprintf(file, "\\begin{small}\n"); 
  fprintf(file, "\\begin{center}\n\\emph{Dealer's up card}\n\\end{center}\n\n");
  
  fprintf(file, "\\begin{tabular}{"); 
  for (j = 0; j <= NUM_CARDS; j++)
    fprintf(file, "c|");
  fprintf(file, "}\n"); 
  
  for (j = 2; j <= NUM_CARDS; j++)
    fprintf(file, "& %d ", j); 
  fprintf(file, "& A \\\\\n"); //Note: Ace is printed on the *right* side 
  
  //Print the hands in the following order: 5 - 21, soft 13 - bust, AA, 22, 
  //then 3,3 - 10,10. 
  for (i = FIVE; i <= TWENTYONE; i++) 
    printHand(i, chart, showWinPct, file); 
  for (i = SOFT_THIRTEEN; i <= BUST; i++)
    printHand(i, chart, showWinPct, file); 
  if (!(MAKE_SIMPLE_CHART))
  {
    printHand(SOFT_TWELVE, chart, showWinPct, file); 
    printHand(FOUR, chart, showWinPct, file); 
    for (i = THREES; i <= TENS; i++)
      printHand(i, chart, showWinPct, file); 
  }
  
  fprintf(file, "\\hline\n\\end{tabular}\n\n"); 
  fprintf(file, "\\end{small}\n\n"); 
  fprintf(file, "\\vspace{.1in}\n"); 
  fprintf(file, "\\noindent KEY:\\\\\nH: Hit\\quad S: Stand\\quad DD: "); 
  fprintf(file, "Double down\\quad SPL: Split\\\\\n"); 
  fprintf(file, "X/Y: X\\%% chance of winning, Y\\%% chance of losing. "
          "(May not add up to 100 due to pushes. For splits, this is the");
  fprintf(file, " probability of winning each of the two split hands.)\n\n");
  
  fprintf(file, "\\end{document}\n"); 
  fclose(file); 
}


//------------------------------------------------------------------------------
// Prints the line of information for a single given hand, hands[i]. 
//------------------------------------------------------------------------------
void printHand (int i, Strategy **chart, int showWinPct, FILE *file)
{
  int j; 
	char *handName;

  if (hands[i].isObvious) //don't print "obvious" hands 
    return; 
    
  fprintf(file, "\\hline\n"); 
	handName = getHandName(hands[i]); 
  fprintf(file, "%s ", handName); 
  
  for (j = 2; j <= NUM_CARDS; j++)
    fprintf(file, " & %s ", actionSymbol(chart[i][j].action)); 
  fprintf(file, " & %s \\\\\n", actionSymbol(chart[i][1].action)); //ace 
  
  if (showWinPct)
  {
    for (j = 2; j <= NUM_CARDS; j++)
      fprintf(file, " & %.0f/%.0f ", 100.*chart[i][j].winPct, 
            100.*chart[i][j].lossPct);
    fprintf(file, " & %.0f/%.0f \\\\\n", 100.*chart[i][1].winPct, 
          100.*chart[i][1].lossPct); //ace 
  }

	free(handName);
}


//------------------------------------------------------------------------------
// Returns the symbol (H, S, DD, SP) corresponding to the given action. 
//------------------------------------------------------------------------------
char * actionSymbol (int action)
{
  if (action == STAND)
    return "S"; 
  else if (action == HIT)
    return "H"; 
  else if (action == SPLIT)
    return "SPL"; 
  else if (action == DOUBLE_DOWN)
    return "DD";   
  else
  {
    throwErr("Unknown action", "actionSymbol"); 
    return ""; 
  }
}


//------------------------------------------------------------------------------
// Prints a Latex chart of the results of running simulations of each possible 
// hand. 
//------------------------------------------------------------------------------
void printSimsChart (HandSim **simsChart, Strategy **chart,  
              const char *filename, int nsims)
{
  FILE *file = NULL; 
  int i, j; 
  
  file = fopen(filename, "w"); 
  if (file == NULL) throwErr("File could not be opened.", "printChart");
  
  fprintf(file, "\\documentclass{article}\n\n"); 
  fprintf(file, "\\usepackage{amsmath, amssymb, color}\n"); 
  fprintf(file, "\\pagenumbering{gobble}\n\n"); 
  fprintf(file, "\\addtolength{\\oddsidemargin}{-.5in}\n"); 
  fprintf(file, "\\addtolength{\\evensidemargin}{-.5in}\n"); 
  fprintf(file, "\\addtolength{\\textwidth}{1in}\n"); 
  fprintf(file, "\\addtolength{\\topmargin}{-1.5in}\n"); 
  fprintf(file, "\\addtolength{\\textheight}{2.3in}\n\n"); 

  fprintf(file, "\\begin{document}\n\n\\begin{center}\n\\begin{large}\n"); 
  fprintf(file, "Blackjack Strategy\n\\end{large}\n\\end{center}\n\n"); 
  
  fprintf(file, "\\begin{small}\n"); 
  fprintf(file, "\\begin{center}\n\\emph{Dealer's up card}\n\\end{center}\n\n");
  
  fprintf(file, "\\begin{tabular}{"); 
  for (j = 0; j <= NUM_CARDS; j++)
    fprintf(file, "c|");
  fprintf(file, "}\n"); 
  
  for (j = 2; j <= NUM_CARDS; j++)
    fprintf(file, "& %d ", j); 
  fprintf(file, "& A \\\\\n"); //Note: Ace is printed on the *right* side 
  
  //Print the hands in the following order: 5 - 21, soft 13 - bust, AA, 22, 
  //then 3,3 - 10,10. 
  for (i = FIVE; i <= TWENTYONE; i++) 
    printHandSims(i, simsChart, chart, file, nsims); 
  for (i = SOFT_THIRTEEN; i <= BUST; i++)
    printHandSims(i, simsChart, chart, file, nsims); 
  printHandSims(SOFT_TWELVE, simsChart, chart, file, nsims); 
  printHandSims(FOUR, simsChart, chart, file, nsims); 
  for (i = THREES; i <= TENS; i++) 
    printHandSims(i, simsChart, chart, file, nsims); 

  fprintf(file, "\\hline\n\\end{tabular}\n\n"); 
  fprintf(file, "\\end{small}\n\n"); 
  
  fprintf(file, "\\end{document}\n"); 
  fclose(file); 
}


//------------------------------------------------------------------------------
// Prints the line of results from simulations for a single hand. 
//------------------------------------------------------------------------------
void printHandSims (int i, HandSim **simsChart, Strategy **chart, FILE *file,
              int nsims)
{
  const double THRESHHOLD = .01; //amount above which to print "errors" in red 
  int j; 
  double winpct, losspct, windiff, lossdiff; 

  if (hands[i].isObvious) //don't print "obvious" hands 
    return; 
    
  fprintf(file, "\\hline\n"); 
  fprintf(file, "%s ", getHandName(hands[i])); 
  
  for (j = 2; j <= NUM_CARDS; j++)
    fprintf(file, " & %s ", actionSymbol(chart[i][j].action)); 
  fprintf(file, " & %s \\\\\n", actionSymbol(chart[i][1].action)); //ace 
  
  for (j = 2; j <= NUM_CARDS; j++)
  {
    winpct = ((double) simsChart[i][j].nwins) / nsims; 
    losspct = ((double) simsChart[i][j].nlosses) / nsims; 
    windiff = winpct - chart[i][j].winPct; 
    lossdiff = losspct - chart[i][j].lossPct; 
    
    fprintf(file, " & "); 
    if (fabs(windiff) > THRESHHOLD) 
    {
      fprintf(file, "\\textcolor{red}{"); 
      fprintf(file, "%c%.0f", windiff > 0. ? '+' : '-', 100. * fabs(windiff)); 
    }
    else
      fprintf(file, "%.0f", 100. * winpct); 
    if (fabs(windiff) > THRESHHOLD) 
      fprintf(file, "}"); 
    fprintf(file, "/"); 
    if (fabs(lossdiff) > THRESHHOLD) 
    {
      fprintf(file, "\\textcolor{red}{"); 
      fprintf(file, "%c%.0f", lossdiff > 0. ? '+' : '-', 100. * fabs(lossdiff)); 
    }
    else
      fprintf(file, "%.0f", 100. * losspct); 
    if (fabs(lossdiff) > THRESHHOLD) 
      fprintf(file, "}"); 
  }
  winpct = ((double) simsChart[i][1].nwins) / nsims; 
  losspct = ((double) simsChart[i][1].nlosses) / nsims; 
  windiff = winpct - chart[i][1].winPct; 
  lossdiff = losspct - chart[i][1].lossPct; 

  fprintf(file, " & "); 
  if (fabs(windiff) > THRESHHOLD) 
  {
    fprintf(file, "\\textcolor{red}{"); 
    fprintf(file, "%c%.0f", windiff > 0. ? '+' : '-', 100. * fabs(windiff)); 
  }
  else
    fprintf(file, "%.0f", 100. * winpct); 
  if (fabs(windiff) > THRESHHOLD) 
    fprintf(file, "}"); 
  fprintf(file, "/"); 
  if (fabs(lossdiff) > THRESHHOLD) 
  {
    fprintf(file, "\\textcolor{red}{"); 
    fprintf(file, "%c%.0f", lossdiff > 0. ? '+' : '-', 100. * fabs(lossdiff)); 
  }
  else
    fprintf(file, "%.0f", 100. * losspct); 
  if (fabs(lossdiff) > THRESHHOLD) 
    fprintf(file, "}"); 

  fprintf(file, " \\\\\n"); 
}





