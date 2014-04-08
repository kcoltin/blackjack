/*
 *  bj_strat.h
 *  Kevin Coltin 
 *
 *  Contains functions used by BlackjackStrategy.c. 
 */

#ifndef BJ_STRAT_H 
#define BJ_STRAT_H 

#include "hands.h" 

#define NUM_CARDS (10) //number of distinct card types: A, 2-10 

//Possible actions to take 
extern const int STAND;
extern const int HIT;
extern const int SPLIT;
extern const int DOUBLE_DOWN;

//Probabilities that the dealer ends up with each possible total 0-22, given 
//his up card, ignoring the possibility of blackjack. 
extern double **dealersProbabilities;

//Represents the strategy a player should take given a certain hand and 
//dealer's up card 
typedef struct {
  int action; //action the player should take: hit, stand, etc. 
  double winPct; //probability of winning the hand, Note 1 
  double lossPct; //prob of losing it 
  double splitEV; //expected value of hand if split 
} Strategy; 

void calculateStrategyChart (Strategy **chart, int MAKE_SIMPLE_CHART); 
int calculateSimpleChart (Strategy **chart); 
int shouldHit (double, double, double, double); 
double probOfWinIgnorePushes (int yourValue, int upCard);
double probOfWinGivenTotal (int, int); 
double probOfLossGivenTotal (int, int); 
double probOfPushGivenTotal (int, int);
double getHitWinProb (Strategy **, int **, int, int);
double getHitLossProb (Strategy **, int **, int, int); 
double getSplitEV (Strategy **, int, int);
double getSplitWinProb (Strategy **, int, int, int **);
double getSplitLossProb (Strategy **, int, int, int **);
double getDDWinProb (Strategy **, Hand, int);
double getDDLossProb (Strategy **, Hand, int);
double * getWinProbsByHand (Strategy **, int, int **);
double * getLossProbsByHand (Strategy **, int, int **);
int doesDealerStand (Hand hand); 
double ** makeDealersProbabilities (); 
double ** makeDealersTransitionMat (); 
double ** makeHitTransitionMat (); 
Hand calculateNewHand (Hand, int); 
double * distribOfHands (int, int); 
double cardProbsAceUpAssumingNoBJ(int);
double cardProbsTenUpAssumingNoBJ(int);
Strategy splitOrDoubleStrat (Strategy **, int, int); 
void computeExpectedValue (Strategy **, double); 
double * getStartingHandProbs (); 
double * getHandExpVals (Strategy **, double); 
double getEVOfHand (Strategy **, int, double); 
double probOfUpCardGivenNoBJ (int); 
double dot_ignore_undef (double *, double *, int);

#endif 

