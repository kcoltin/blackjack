/*
 *  main.c
 *  Kevin Coltin 
 *
 *  Computes the optimal strategy for each possible hand in Blackjack and 
 *  produces a chart showing what a player should do with each hand. 
 *
 *  Use: 
 *  To execute the program, which will compute the optimal strategy and output
 *  results in a chart: 
 *  ./BlackjackStrategy
 * 
 *  To run Monte Carlo simulations to numerically verify the optimal strategy:
 *  ./BlackjackStrategy sims
 * 
 *  Assumptions: 
 *  Doubling down and splitting are allowed. 
 *  Dealer hits soft 17. 
 *   There are several decks, so the probability of drawing any particular card 
 *    (ignoring suits) may always be taken to be 1/13. 
 */

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "util/error.h"
#include "util/boolean.h"
#include "util/linal.h"
#include "bj_strat.h"
#include "hands.h" 
#include "print_chart.h" 

void compute_strategy (); 
void run_sims ();

int main (int argc, char **argv)
{
  if (argc >= 2 && !strcmp(argv[1], "sims"))  
    run_sims ();  
  else 
    compute_strategy ();

  return 0; 
}



// Main body of the program, for computing strategy
void compute_strategy ()
{
  //File name to print chart to 
  const char *filename = "Blackjack strategy chart.tex"; 
  //Indicates whether to show the win percentage for each hand in the chart
  const int SHOW_WIN_PCT = TRUE;
  //Indicates whether to exclude doubles and splits 
  const int MAKE_SIMPLE_CHART = FALSE;
  //Ratio of the player's bet that he wins by getting blackjack 
  const double BLACKJACK_PAYS = 3./2.; 
  
  Strategy **chart = NULL; 
  int i; 
  
  //chart is a NUM_HANDS by NUM_CARDS+1 matrix, with entry i,j being hands[i] 
  //and the card with face value j. 
  chart = (Strategy **) malloc(NUM_HANDS * sizeof(Strategy *)); 
  for (i = 0; i < NUM_HANDS; i++)
    chart[i] = (Strategy *) malloc((NUM_CARDS+1) * sizeof(Strategy)); 
  if (chart == NULL) throwMemErr("chart", "main"); 
  
  //Make vector of possible hands 
  makeHands(); 
  //Make matrix of dealer's probabilities of ending up with a given total given
  //each given up card 
  dealersProbabilities = makeDealersProbabilities(); 
  
  //Compute optimal strategy for each combination of player's hand and 
  //dealer's up card 
  calculateStrategyChart (chart, MAKE_SIMPLE_CHART); 
  
  //Print chart to Latex   
  printChart (chart, filename, SHOW_WIN_PCT, MAKE_SIMPLE_CHART); 
  printf("Program complete.\n"); 
  
  //Compute player's expected value 
  if (!(MAKE_SIMPLE_CHART))
    computeExpectedValue (chart, BLACKJACK_PAYS); 
  
  freematrix(dealersProbabilities, NUM_CARDS+1); 
  for (i = 0; i < NUM_HANDS; i++)
    free(chart[i]); 
  free(chart); 
}


//Runs Monte Carlo simulations to test the strategy
void run_sims ()
{
  //File name to print chart to 
  const char *filename = "Simulations chart.tex"; 
  //Number of simulations to run for each combination of hand and up card (to
  //start with; user may add more simulations during the running of the
  //program) 
  const int N_SIMS = 1000; 

  Strategy **chart = NULL; 
  HandSim **simsChart; 
  int i, j; 
  int N; //number of simulations to run 
  int n; //number that have been completed 
  int m; //number of additional sims to run 
  int info; 
  
  //First, compute the strategy chart in the same way as BlackjackStrategy.c. 
  
  //chart is a NUM_HANDS by NUM_CARDS+1 matrix, with entry i,j being hands[i] 
  //and the card with face value j. 
  chart = (Strategy **) malloc(NUM_HANDS * sizeof(Strategy *)); 
  for (i = 0; i < NUM_HANDS; i++)
    chart[i] = (Strategy *) malloc((NUM_CARDS+1) * sizeof(Strategy)); 
  if (chart == NULL) throwMemErr("chart", "main"); 
  
  //Make vector of possible hands 
  makeHands(); 
  //Make matrix of dealer's probabilities of ending up with a given total given
  //each given up card 
  dealersProbabilities = makeDealersProbabilities(); 
  
  //Compute optimal strategy for each combination of player's hand and 
  //dealer's up card 
  calculateStrategyChart (chart, FALSE); 

  simsChart = initializeSimsChart(); 
  
  n = 0; 
  N = N_SIMS; 
  
  while (n < N)
  {
    for (i = 0; i < NUM_HANDS; i++) //i = player's hand 
    {
      if (!(hands[i].isObvious))
      {
        for (j = 1; j <= NUM_CARDS; j++) //j = up card 
        {
          runSims (simsChart, chart, i, j, N - n); 
        }
      }
      
      printf("Finished running simulations for hand %d of %d.\n", 
          i + 1, NUM_HANDS); 
    }
    
    n = N; 
    
    //Print chart to Latex   
    printSimsChart (simsChart, chart, filename, n); 
    
    printf("Completed %d simulations for each hand. Maximum error: %.0f%%."
      "\nHow many more simulations would you like to run? (Enter N=0 to "
      "finish.)\nN = ", n, fmax(getMaxWinErr(chart, simsChart, n), 
      getMaxLossErr(chart, simsChart, n))); 
    info = scanf("%d", &m); 
	if (info != 1) throwErr("Error with scanf", "run_sims"); 
    printf("\n"); 

    N += m; 
  }
  
  freematrix(dealersProbabilities, NUM_CARDS+1); 
  for (i = 0; i < NUM_HANDS; i++)
    free(chart[i]); 
  free(chart); 
  for (i = 0; i < NUM_HANDS; i++)
    free(simsChart[i]); 
  free(simsChart); 
}









