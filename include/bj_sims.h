/*
 *  bj_sims.h
 *  Kevin Coltin 
 *
 *  Contains functions used by BlackjackSimulations.c. 
 */

#ifndef BJ_SIMS_H 
#define BJ_SIMS_H 

#include "bj_strat.h" 
#include "hands.h"

//Structure representing the results of simulations for a single combination of
//player's hand and dealer's up card 
typedef struct {
  int nwins; 
  int nlosses; 
} HandSim; 


//Function prototypes 
void runSims(HandSim **simsChart, Strategy **chart, int i, int upCard, int N);
int doesPlayerWin (int playerTotal, int dealerTotal);
int doesPlayerLose (int playerTotal, int dealerTotal);
HandSim ** initializeSimsChart (); 
HandSim newHandSim (); 
double getMaxWinErr(Strategy **chart, HandSim **simsChart, int nsims); 
double getMaxLossErr(Strategy **chart, HandSim **simsChart, int nsims); 
void removeCardsInStartingHand (int *deck, Hand hand, int cardsInDeck); 
int * chooseCardsInStartingHand (int *deck, int value, int cardsInDeck); 

#endif
