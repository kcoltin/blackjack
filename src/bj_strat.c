#include "bj_strat.h"
#include <stdlib.h> 
#include "boolean.h"
#include "error.h"
#include "linal.h"
#include "moremath.h"
#include "hands.h" 

double **dealersProbabilities; 
const int STAND = 1; 
const int HIT = 2; 
const int SPLIT = 3; 
const int DOUBLE_DOWN = 4; 

//Number of possible outcomes - 0 through 22, with 22 signifying bust. The 
//reason for including numbers under 12 is just so that the indices of an 
//outcomes vector are the same as the point value. 
const static int NUM_OUTCOMES = 23; 

//Maximum number of cards after the first two that it is theoretically possible
//to get. (After the 22nd card you are guaranteed to go bust.) The point of 
//this is to get the long term final transition matrix, P raised to this 
//power. 
const static int MAX_POSSIBLE_HITS = 22; 

//Probability of drawing each card (in order, A, 2-10). Note: A dummy value of 
//zero is added to the front so that CARD_PROBABILITIES[k] is the probability 
//of getting card with value k. 
static double CARD_PROBABILITIES[NUM_CARDS+1] = {0., 1./13, 1./13, 1./13, 1./13, 1./13,
                                                 1./13, 1./13, 1./13, 1./13, 4./13}; 

static double **hitTransitionMatrix; 


//------------------------------------------------------------------------------
// Creates the chart. If MAKE_SIMPLE_CHART is true, ignores splits and doubles. 
//------------------------------------------------------------------------------
void calculateStrategyChart (Strategy **chart, int MAKE_SIMPLE_CHART)
{
  int i, j, equivIndex; 
  Hand doubleHand, equivHand; 
  int status; 

  //First, calculate strategies ignoring non-simple hands, and ignoring 
  //splits/doubles. 
  status = calculateSimpleChart (chart); 
  
  //Copy strategies from simple chart on to non-simple hands 
  for (i = THREES; i <= TENS; i++)
  {
    doubleHand = hands[i]; 
    equivHand = makeHand (doubleHand.value, FALSE, FALSE, FALSE); 
    equivIndex = getHandIndex(equivHand); 
    
    for (j = 1; j <= NUM_CARDS; j++)
      chart[i][j] = chart[equivIndex][j]; 
  }
  if (status == EXIT_SUCCESS && !(MAKE_SIMPLE_CHART)) //Note 2 
  {
    //Then, determine when to split or double 
    for (i = 0; i < NUM_HANDS; i++)
      for (j = 1; j <= NUM_CARDS; j++)
        chart[i][j] = splitOrDoubleStrat (chart, i, j);   
  }
  else if (MAKE_SIMPLE_CHART)
  {
    for (i = THREES; i < NUM_HANDS; i++)
      hands[i].isObvious = TRUE; //hide splits 
  }
}


  
//------------------------------------------------------------------------------
// Calculates the strategy chart ignoring splits or doubles. 
// Note: For all purposes of this function and sub-functions, the only relevant
// probability is the probability of winning conditioned on the event that it
// is either a win or a loss - i.e. pushes are ignored. 
//------------------------------------------------------------------------------
int calculateSimpleChart (Strategy **chart)
{ 
  int status; 
  Hand hand; 
  int i, j, upCard; 
  double hitWinProb, standWinProb; //probabilities of winning the hand if you 
                        //hit or stand, respectively 
  double hitLossProb, standLossProb; //same 
  
  int doneCount = 0; //count of number of hands that we have solved for 
  int COUNT_END = NUM_HANDS_SIMPLE * NUM_CARDS;   //total hands to solve 
  
  //Flag to ensure an infinite loop isn't created. The flag will be set to true
  //every time we restart going through all the possible hands, and is set to  
  //false if at least one possibility is "solved" on that run-through. 
  int infLoop; 

  //Boolean matrix indicating which strategies have been solved for: 
  //entry i,j corresponds to the i,j scenario in chart. 
  int **isSolved = NULL; 
  isSolved = (int **) malloc (NUM_HANDS_SIMPLE * sizeof(int *)); 
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
    isSolved[i] = (int *) malloc((NUM_CARDS+1)*sizeof(int)); 
  if (isSolved == NULL) throwMemErr("isSolved", "calculateSimpleChart");  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
    for (j = 1; j <= NUM_CARDS; j++)
      isSolved[i][j] = FALSE; 

  hitTransitionMatrix = makeHitTransitionMat (); 
    
  //Make bust row 
  for (j = 1; j <= NUM_CARDS; j++)
  {
    chart[BUST][j].action = STAND; 
    chart[BUST][j].winPct = 0.; 
    chart[BUST][j].lossPct = 1.; 
    isSolved[BUST][j] = TRUE; 
    doneCount++; 
  }
  
  do 
  {
    infLoop = TRUE; 
  
    for (i = NUM_HANDS_SIMPLE - 1; i >= 0; i--) //NOTE 3 
    {
      if (i == BUST) //already solved bust above 
        continue; 
    
      hand = hands[i]; 
      
      for (upCard = NUM_CARDS; upCard >= 1; upCard--)
      {
        if (!(isSolved[i][upCard])) //skip strats that are already solved 
        {
          standWinProb = probOfWinGivenTotal (hand.value, upCard); 
          standLossProb = probOfLossGivenTotal (hand.value, upCard); 
          
          //Compute probability of winning and losing if you hit 
          hitWinProb = getHitWinProb (chart, isSolved, i, upCard);
          hitLossProb = getHitLossProb (chart, isSolved, i, upCard); 
          
          //If getHitWinProb was successful, hitWinProb will not equal -1. 
          if (hitWinProb >= 0.) 
          {
            //This condition should be impossible - just checking to make 
            //sure. 
            if (hitLossProb < 0.)
              throwErr("getHitWinProb exited successfully, but "
                "getHitLossProb did not.", "calculateSimpleChart"); 
          
            if (shouldHit(hitWinProb, hitLossProb, 
              standWinProb, standLossProb)) 
            {
              chart[i][upCard].action = HIT; 
              chart[i][upCard].winPct = hitWinProb; 
              chart[i][upCard].lossPct = hitLossProb; 
            }
            else 
            {
              chart[i][upCard].action = STAND; 
              chart[i][upCard].winPct = standWinProb; 
              chart[i][upCard].lossPct = standLossProb; 
            }
            
            doneCount++; 
            infLoop = FALSE; 
            isSolved[i][upCard] = TRUE; 
          }
          
          //If it is not a success, continue iterating through the 
          //possibilities. 
        }
      }
    }
  }
  while (doneCount < COUNT_END && !(infLoop)); 

  //If it doesn't converge, replace every non-converged scenario with "double 
  //down", which serves as a flag for debugging. 
  if (infLoop)
  {
    status = EXIT_FAILURE; 
    warning("Infinite loop created - method failed to converge.", 
          "calculateSimpleChart"); 
    
    for (i = 0; i < NUM_HANDS_SIMPLE; i++)
    {
      for (j = 1; j <= NUM_HANDS; j++)
      {
        if (!(isSolved[i][j]))
        {
          chart[i][j].action = DOUBLE_DOWN; 
          chart[i][j].winPct = 0.; 
          chart[i][j].lossPct = 0.; 
        }
      }
    }
  }
  else
    status = EXIT_SUCCESS; 

  for (i = 0; i < NUM_HANDS_SIMPLE; i++) {
		free(isSolved[i]);
	}
	free(isSolved);

  return status; 
}



//------------------------------------------------------------------------------
// Indicates whether the player should hit or stand given the probabilities of 
// winning or losing. Works to maximize expected value. 
// E(payoff | hit) = hitWinProb * bet - hitLossProb * bet 
// E(payoff | stand) = standWinProb * bet - standLossProb * bet 
// E(payoff | hit) > E(payoff | stand) iff
//    hitWinProb - hitLossProb > standWinProb - standLossProb. 
// The same holds replacing > with < or =. 
// In a tie, defer to the one with the highest probability of a push, so as to 
// minimize variance over many hands played. 
//------------------------------------------------------------------------------
int shouldHit (double hitWinProb, double hitLossProb, double standWinProb, 
          double standLossProb)
{
  double diff = (hitWinProb - hitLossProb)  - (standWinProb - standLossProb); 
  int hit; 
  
  if (diff == 0.)
    hit = (hitWinProb < standWinProb); //push prob is greater for hit 
  else
    hit = (diff > 0.); 
  
  return hit; 
}



//------------------------------------------------------------------------------
// Computes the probability that you will win the hand, given the total ending
// value of your hand and the dealer's up card, and conditioned on the event 
// that there is not a push.
// Note: This gives the conditional probabilities given that neither the player 
// nor the dealer has blackjack. 
//------------------------------------------------------------------------------
double probOfWinIgnorePushes (int yourValue, int upCard)
{
  double p = probOfWinGivenTotal (yourValue, upCard); 
  double q = probOfLossGivenTotal (yourValue, upCard); 
  return p / (p + q); 
}



//------------------------------------------------------------------------------
// Computes the probability that you will win the hand, given the total ending
// value of your hand and the dealer's up card. Note: This gives the conditional
// probabilities given that neither the player nor the dealer has blackjack. 
// This does not include the probability of a push. 
//------------------------------------------------------------------------------
double probOfWinGivenTotal (int yourValue, int upCard)
{
  double prob; 
  const int DEALERS_MIN = 17; //dealer always ends up with at least 17 
  int i; 

  if (yourValue > 21)
    prob = 0.; 
  else if (yourValue <= DEALERS_MIN) 
    prob = dealersProbabilities[upCard][BUST_VALUE]; //win iff dealer busts
  else //win iff dealer busts or your total is higher 
  {
    prob = dealersProbabilities[upCard][BUST_VALUE]; 
    for (i = DEALERS_MIN; i < yourValue; i++)
      prob += dealersProbabilities[upCard][i]; 
  }  
  
  return prob; 
}



//------------------------------------------------------------------------------
// Computes the probability that you will lose the hand, given the total ending
// value of your hand and the dealer's up card. Note: This gives the conditional
// probabilities given that neither the player nor the dealer has blackjack. 
// This does not include the probability of a push. 
//------------------------------------------------------------------------------
double probOfLossGivenTotal (int yourValue, int upCard)
{
  const int DEALERS_MIN = 17; //dealer always ends up with at least 17 
  int i; 
  double prob = 0.; 
  
  if (yourValue > 21)
    prob = 1.; 
  else
  { 
    for (i = maxi(yourValue + 1, DEALERS_MIN); i <= 21; i++)
      prob += dealersProbabilities[upCard][i]; 
  }
  
  return prob; 
}



//------------------------------------------------------------------------------
// Probability that the hand will be a push, given the total ending value of 
// your hand and the dealer's up card. 
//------------------------------------------------------------------------------
double probOfPushGivenTotal (int yourValue, int upCard)
{
  return 1. - probOfWinGivenTotal(yourValue, upCard) 
        - probOfLossGivenTotal(yourValue, upCard); 
  
}



//------------------------------------------------------------------------------
// Returns the probability of winning the hand if the player hits. The 
// probability is given by the dot product of the row of the transition matrix
// corresponding to the player's hand and the vector of probabilities of winning
// given each hand his hand could transform to after the hit. 
// This is the dot product of P[handIndex] and the winPct's of chart[:][upCard].
// 
// The hit transition matrix is the transition matrix of moving from 
// the current hand to another if the player hits on the current hand. 
// 
// Note: If the chart is not complete enough for this to be computed, it returns
// an error code of -1. 
//------------------------------------------------------------------------------
double getHitWinProb (Strategy **chart, int **isSolved, int handIndex, 
              int upCard)
{
  double *winProbsByHand = getWinProbsByHand(chart, upCard, isSolved); 
  int i; 
  const double ERR_CODE = -1.; 
  double p; 
  
  //Check to see if the problem is solvable 
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
  {
    if (hitTransitionMatrix[handIndex][i] > 0. && !(isSolved[i][upCard])) {
  		free(winProbsByHand); 
      return ERR_CODE; 
		}
  }
  
  p = dot_ignore_undef (hitTransitionMatrix[handIndex], winProbsByHand, 
	                      NUM_HANDS_SIMPLE); 
  free(winProbsByHand); 
  
  return p; 
}


//------------------------------------------------------------------------------
// Same as getHitWinProb, but probability of loss. 
//------------------------------------------------------------------------------
double getHitLossProb (Strategy **chart, int **isSolved, int handIndex, 
                int upCard)
{
  double *lossProbsByHand = getLossProbsByHand(chart, upCard, isSolved); 
  int i; 
  const double ERR_CODE = -1.; 
  double p; 
  
  //Check to see if the problem is solvable 
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
  {
    if (hitTransitionMatrix[handIndex][i] > 0. && !(isSolved[i][upCard])) {
  		free(lossProbsByHand); 
      return ERR_CODE; 
		}
  }
  
  p = dot_ignore_undef (hitTransitionMatrix[handIndex], lossProbsByHand, 
	                      NUM_HANDS_SIMPLE); 
  free(lossProbsByHand); 
  
  return p; 
}


//------------------------------------------------------------------------------
// Returns the player's expected value of a hand if he splits his cards on the 
// hand. 
//------------------------------------------------------------------------------
double getSplitEV (Strategy **chart, int splitCard, int upCard)
{
  double ev, p_hand, ev_hand; 
  int i; 
  Strategy strat; 

  //Distribution of what the "starting hand" will be - i.e. the hand consisting
  //of one of the two original split cards and the first new card that is 
  //dealt to it. 
  double *startingHandDistrib = distribOfHands (splitCard, FALSE); 
  
  //index of the hand that you would split - the hand consisting of two 
  //of "splitCard"
  int splittableHandIndex = getHandIndex(getHandByCards(splitCard, splitCard,
                                      FALSE)); 

// These equations give the expected value of *each* newly split hand:   
// ev = p_hand1 * ev_hand1 + p_hand1 * ev_hand1 + ... + p_same * 2 * ev 
// ev = (p_hand1 * ev_hand1 + ... ) / (1 - 2 * p_same) 
  ev = 0.; 
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
  {
    if (hands[i].isSplittable) //skip 2,2 and A,A 
      continue; 
  
    //Add the probability of getting hand i times the expected value of 
    //hand i 
    p_hand = startingHandDistrib[i]; 
    
    strat = chart[i][upCard]; 
    ev_hand = strat.winPct - strat.lossPct; 
    
    //If the strategy is to double down, expected value is multiplied by two. 
    //(This is true for splits as well of course, but that doesn't apply here
    //because a splittable hand cannot become another splittable hand. 
    if (strat.action == DOUBLE_DOWN)
      ev_hand *= 2.; 
    
    ev += p_hand * ev_hand; 
  }
    
  ev /= 1. - 2. * startingHandDistrib[splittableHandIndex]; 

  //Multiply by two: what we have until now is the exp. val of *each* split hand
  ev *= 2.; 
  
	free(startingHandDistrib); 
  return ev; 
}


//------------------------------------------------------------------------------
// Returns the probability of winning one of the hands resulting from a split. 
// I.e. if a player has AA and splits into two hands, and we call one of those 
// hands H, then this is the probability of winning hand H. splitCard is the 
// card that was split - i.e., the original hand was two cards both of type 
// splitCard. 
//------------------------------------------------------------------------------
double getSplitWinProb (Strategy **chart, int splitCard, int upCard, int **isSolved)
{
  double *winProbsByHand = getWinProbsByHand(chart, upCard, isSolved); 
  double p; 
  int i; 

  //index of the hand consisting of two of "splitCard" 
  int splitHandIndex = getHandIndex(getHandByCards(splitCard,splitCard,FALSE));

  //Distribution of what the "starting hand" will be - i.e. the hand consisting
  //of one of the two original split cards and the first new card that is 
  //dealt to it. 
  double *startingHandDistrib = distribOfHands (splitCard, FALSE); 

  //probability that the next hand will be the same as the original hand - i.e.
  //that a third card of type splitCard will be dealt. 
  double sameProb = startingHandDistrib[splitHandIndex]; 
  
  startingHandDistrib[splitHandIndex] = 0.;   
  for (i = 0; i < NUM_HANDS; i++)
    startingHandDistrib[i] /= 1. - sameProb; 
    
  //This is technically the probability of winning each split hand given that 
  //the split hand does not become the original hand. E.g., if splitCard = 8,
  //it is the probability that the new hand resulting from one 8 will win 
  //given that the next hand dealt to the new hand is *not* another 8. The 
  //formula is: 
  //winpct = (p_hand1 * pwin_hand1 + p_hand2 * pwin_hand2 + ...) / (1 - p_same)
  p = dot (startingHandDistrib, winProbsByHand, NUM_HANDS_SIMPLE);
    
  free(winProbsByHand); 
  free(startingHandDistrib); 
  
  return p; 
}


//------------------------------------------------------------------------------
// Like getSplitWinProb but for a loss. 
//------------------------------------------------------------------------------
double getSplitLossProb (Strategy **chart, int splitCard, int upCard, int **isSolved)
{
  double *lossProbsByHand = getLossProbsByHand(chart, upCard, isSolved); 
  double p; 
  int i; 

  //index of the hand consisting of two of "splitCard" 
  int splitHandIndex = getHandIndex(getHandByCards(splitCard,splitCard,FALSE));

  //Distribution of what the "starting hand" will be - i.e. the hand consisting
  //of one of the two original split cards and the first new card that is 
  //dealt to it. 
  double *startingHandDistrib = distribOfHands (splitCard, FALSE); 

  //probability that the next hand will be the same as the original hand - i.e.
  //that a third card of type splitCard will be dealt. 
  double sameProb = startingHandDistrib[splitHandIndex]; 
  
  startingHandDistrib[splitHandIndex] = 0.;   
  for (i = 0; i < NUM_HANDS; i++)
    startingHandDistrib[i] /= 1. - sameProb; 
    
  //This is technically the probability of lossning each split hand given that 
  //the split hand does not become the original hand. E.g., if splitCard = 8,
  //it is the probability that the new hand resulting from one 8 will loss 
  //given that the next hand dealt to the new hand is *not* another 8. The 
  //formula is: 
  //losspct = (p_hand1 * ploss_hand1 + p_hand2 * ploss_hand2 + ...) / (1 - p_same)
  p = dot (startingHandDistrib, lossProbsByHand, NUM_HANDS_SIMPLE);
    
  free(lossProbsByHand); 
  free(startingHandDistrib); 
  
  return p; 
}




//------------------------------------------------------------------------------
// Probability of winning the hand if the player doubles down. 
//------------------------------------------------------------------------------
double getDDWinProb (Strategy **chart, Hand hand, int upCard)
{
  int i; 
  int handIndex; 
  double p; 
  double *winProbs = NULL; //probabilities of winning a hand given your total 
                  //and the up card 
  double *totalProbs = NULL; //probabilities of ending up with the given 
                    //total card value 
  
  //Convert a splittable hand to the equivalent non-splittable type 
  if (hand.isSplittable && !(areHandsEqual(hand, hands[FOUR])
                    || areHandsEqual(hand, hands[TWELVE]))) 
  {
    hand = makeHand (hand.value, FALSE, FALSE, FALSE); 
  }
  
  handIndex = getHandIndex(hand); 
  
  winProbs = allocvector(NUM_OUTCOMES); 
  if (winProbs == NULL) throwMemErr("winProbs", "getDDWinProb"); 
  
  for (i = 0; i < NUM_OUTCOMES; i++)
    winProbs[i] = probOfWinGivenTotal(i, upCard); 
  
  //The distribution of what the final hand will be is given by the row 
  //of the hit transition matrix corresponding to "hand". 
  //We need to convert this distribution of hands into a distribution of 
  //the values of those hands. 
  totalProbs = zerosv(NUM_OUTCOMES); 
  if (totalProbs == NULL) throwMemErr("totalProbs", "getDDWinProb"); 
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
    totalProbs[hands[i].value] += hitTransitionMatrix[handIndex][i];   
  
  p = dot (totalProbs, winProbs, NUM_OUTCOMES); 
  free(totalProbs); 
  free(winProbs); 
  
  return p; 
}


//------------------------------------------------------------------------------
// Probability of losing the hand if the player doubles down. 
//------------------------------------------------------------------------------
double getDDLossProb (Strategy **chart, Hand hand, int upCard)
{
  int i; 
  int handIndex; 
  double p; 
  double *lossProbs = NULL; //probabilities of losing a hand given your total 
                  //and the up card 
  double *totalProbs = NULL; //probabilities of ending up with the given 
                    //total card value 
  
  //Convert a splittable hand to the equivalent non-splittable type 
  if (hand.isSplittable && !(areHandsEqual(hand, hands[FOUR])
                    || areHandsEqual(hand, hands[TWELVE]))) 
  {
    hand = makeHand (hand.value, FALSE, FALSE, FALSE); 
  }
  
  handIndex = getHandIndex(hand); 
  
  lossProbs = allocvector(NUM_OUTCOMES); 
  if (lossProbs == NULL) throwMemErr("lossProbs", "getDDLossProb"); 
  
  for (i = 0; i < NUM_OUTCOMES; i++)
    lossProbs[i] = probOfLossGivenTotal(i, upCard); 
  
  //The distribution of what the final hand will be is given by the row 
  //of the hit transition matrix corresponding to "hand". 
  //We need to convert this distribution of hands into a distribution of 
  //the values of those hands. 
  totalProbs = zerosv(NUM_OUTCOMES); 
  if (totalProbs == NULL) throwMemErr("totalProbs", "getDDLossProb"); 
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
    totalProbs[hands[i].value] += hitTransitionMatrix[handIndex][i];   
  
  p = dot (totalProbs, lossProbs, NUM_OUTCOMES); 
  free(totalProbs); 
  free(lossProbs); 
  
  return p; 
}


//------------------------------------------------------------------------------
// Returns the probability of winning each hand given the dealer's up card. 
//------------------------------------------------------------------------------
double * getWinProbsByHand (Strategy **chart, int upCard, int **isSolved)
{
  int i; 
  double *winProbsByHand = allocvector(NUM_HANDS_SIMPLE); 
  if(winProbsByHand == NULL) throwMemErr("winProbsByHand", "getWinProbsByHand");
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++) {
		if (isSolved[i][upCard]) {
	    winProbsByHand[i] = chart[i][upCard].winPct; 
		}
		else {
			// signals that it is not yet defined, since probabilities can't be neg.
			winProbsByHand[i] = -1.; 
		}
	}
  
  return winProbsByHand; 
}  


//------------------------------------------------------------------------------
// Returns the probability of losing each hand given the dealer's up card. 
//------------------------------------------------------------------------------
double * getLossProbsByHand (Strategy **chart, int upCard, int **isSolved)
{
  int i; 
  double *lossProbsByHand = allocvector(NUM_HANDS_SIMPLE); 
  if (lossProbsByHand == NULL) 
    throwMemErr("lossProbsByHand", "getLossProbsByHand");
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++) {
		if (isSolved[i][upCard]) {
	    lossProbsByHand[i] = chart[i][upCard].lossPct; 
		}
		else {
			// signals that it is not yet defined, since probabilities can't be neg.
			lossProbsByHand[i] = -1.; 
		}
	}
  
  return lossProbsByHand; 
}  


//------------------------------------------------------------------------------
// Indicates whether the dealer stands on a given hand.  If it returns false, 
// the dealer hits. 
//------------------------------------------------------------------------------
int doesDealerStand (Hand hand)
{
  if (hand.value >= 18) //always stands with 18 or more 
    return TRUE; 
  else if (hand.value <= 16) //always hits with 16 or fewer 
    return FALSE; 
  else 
    return !(hand.isSoft); //hits on soft 17, stands on hard 17
}


//------------------------------------------------------------------------------
// Makes a matrix giving the probability that the dealer will end up with a 
// given total given his up card.  Each row corresponds to an up card 1 - 10 
// (with the first row blank in order for the index to correspond with the card
// number), and each column is a value 0-22, with 22 corresponding to bust. 
//------------------------------------------------------------------------------
double ** makeDealersProbabilities ()
{
  int upCard; 
  int j; 
  double *pi = NULL; 
  double *v = NULL; 
  double **P, **PP; 
  double **dealerProbabilities = NULL; 
  
  dealerProbabilities = zerosm(NUM_CARDS+1, NUM_OUTCOMES); 
  if (dealerProbabilities == NULL) 
    throwMemErr("dealerProbabilities", "makeDealerProbabilities"); 
  
  P = makeDealersTransitionMat(hands); 
  
  //For each possible dealer's up card, compute probability that dealer will
  //end up with a given total 
  for (upCard = 1; upCard <= NUM_CARDS; upCard++)
  {
    //Compute pi, the distribution vector of the dealer's possible hands 
    //given his up card 
    pi = distribOfHands(upCard, TRUE); 
    
    //v = pi*P^22. v is the distribution vector of the hands the dealer 
    //could end up with. 
    PP = matrixpow(P, NUM_HANDS_SIMPLE, MAX_POSSIBLE_HITS); 
    v = vtimesm(pi, PP, NUM_HANDS_SIMPLE, NUM_HANDS_SIMPLE); 
    
    //Convert the probabilities of ending up with each hand into the 
    //probabilities of ending up with each value 
    for (j = 0; j < NUM_HANDS_SIMPLE; j++)
      dealerProbabilities[upCard][hands[j].value] += v[j]; 

	  free(pi); 
	  free(v); 
  	freematrix(PP, NUM_HANDS_SIMPLE); 
  }

  freematrix(P, NUM_HANDS_SIMPLE); 
  
  return dealerProbabilities; 
}


//------------------------------------------------------------------------------
// Makes the Markov transition matrix showing the probability of the dealer's 
// next hand being a given hand given his current hand. 
//------------------------------------------------------------------------------
double ** makeDealersTransitionMat ()
{
  double **P = NULL; 
  Hand newHand; 
  int i, j, k; 

  P = zerosm(NUM_HANDS_SIMPLE, NUM_HANDS_SIMPLE); 
  if (P == NULL) throwMemErr("P", "makeDealersTransitionMat"); 
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
  {
    if (doesDealerStand(hands[i]))
      P[i][i] = 1.; //dealer never moves away from this hand 
    else //if dealer draws a card 
    {
      for (k = 1; k <= NUM_CARDS; k++)
      {
        //new hand that will be obtained from hand i by drawing card k 
        newHand = calculateNewHand (hands[i], k); 
    
        //probability of moving to hand j is the probability of drawing card k
        j = getHandIndex(newHand); 
        P[i][j] += CARD_PROBABILITIES[k]; 
      }
    }
  }
  
  return P; 
}



//------------------------------------------------------------------------------
// Makes the Markov transition matrix showing the probability of the player's 
// next hand being a given hand given his current hand and given that he hits
// on the current hand. Only includes simple hands. 
//------------------------------------------------------------------------------
double ** makeHitTransitionMat ()
{
  double **P = NULL; 
  Hand newHand; 
  int i, j, k; 

  P = zerosm(NUM_HANDS_SIMPLE, NUM_HANDS_SIMPLE); 
  if (P == NULL) throwMemErr("P", "makeHitTransitionMat"); 
  
  for (i = 0; i < NUM_HANDS_SIMPLE; i++)
  {
    if (i == BUST)
      P[i][i] = 1.; //player never moves away from a bust 
    else 
    {
      for (k = 1; k <= NUM_CARDS; k++)
      {
        //new hand that will be obtained from hand i by drawing card k 
        newHand = calculateNewHand (hands[i], k); 
    
        //probability of moving to hand j is the probability of drawing card k
        j = getHandIndex(newHand); 
        P[i][j] += CARD_PROBABILITIES[k]; 
      }
    }
  }
  
  return P; 
}



//------------------------------------------------------------------------------
// Computes the new hand that results from oldHand by drawing a given card. 
//------------------------------------------------------------------------------
Hand calculateNewHand (Hand oldHand, int card)
{
  Hand newHand = {oldHand.value + card, oldHand.isSoft, FALSE, FALSE}; 
  
  //If the new card is an ace, count it as 11 initially 
  if (card == 1)
  {
    newHand.value += 10; 
    newHand.isSoft = TRUE; 
  }
  
  //If there is either an ace being used as 11 in the original hand, and/or the
  //new card is an ace, count it as 1 if necessary 
  if (newHand.isSoft && newHand.value > 21)
  {
    newHand.value -= 10; 
    
    //The new hand is not soft, unless the previous hand was soft and the new
    //card is an ace. 
    newHand.isSoft = oldHand.isSoft && card == 1; 
  }
  
  if (newHand.value > 21)
    newHand = hands[BUST]; 
  
  //Return the hand in the hands array that is equal to the newly created hand
  return getHand(newHand); 
}


//------------------------------------------------------------------------------
// Creates a vector of the probability distribution of the dealer's hands, given
// his up card, or (depending on the value of the boolean argument isDealer) 
// the probability distribution of the hand resulting from a split hand, where
// knownCard is the card that was split. 
// The boolean argument isDealer indicates whether this is for the dealer's
// hand - if so, it assumes no blackjack and ignores splittable hands. 
// The resulting vector has length NUM_HANDS if isDealer = false and 
// NUM_HANDS_SIMPLE if isDealer is true. 
//------------------------------------------------------------------------------
double * distribOfHands (int knownCard, int isDealer)
{
  double *pi = NULL; //vector of possible hands 
  int downCard, newCard; 
  Hand hand; 
  int index; 
  
  if (isDealer)
  {
    pi = zerosv(NUM_HANDS_SIMPLE); 
    if (pi == NULL) throwMemErr("pi", "distribOfHands"); 
    
    for (downCard = 1; downCard <= NUM_CARDS; downCard++)
    {
      //Get the hand that the dealer has, comprised of his up and down cards
      hand = getHandByCards(knownCard, downCard, TRUE); 
      index = getHandIndex(hand); 
      
      if (knownCard == 1)
        pi[index] += cardProbsAceUpAssumingNoBJ(downCard); 
      else if (knownCard == 10)
        pi[index] += cardProbsTenUpAssumingNoBJ(downCard); 
      else
        pi[index] += CARD_PROBABILITIES[downCard]; 
    }
  }
  else
  {
    pi = zerosv(NUM_HANDS); 
    if (pi == NULL) throwMemErr("pi", "distribOfHands"); 
    
    for (newCard = 1; newCard <= NUM_CARDS; newCard++)
    {
      hand = getHandByCards(knownCard, newCard, FALSE); 
      index = getHandIndex(hand); 
      pi[index] += CARD_PROBABILITIES[newCard]; 
    }
  }
  
  return pi; 
}


//------------------------------------------------------------------------------
// Gives the probability that the dealer has the given down card, given that his
// up card is an Ace and given that the down card is not a ten. 
//------------------------------------------------------------------------------
double cardProbsAceUpAssumingNoBJ (int downCard)
{
  if (downCard == 10)
    return 0.; 
  else
    return 1./9.; //There are 9 equally likely possibilities. 
}


//------------------------------------------------------------------------------
// Gives the probability that the dealer has the given down card, given that his
// up card is a ten and given that the down card is not an ace. 
//------------------------------------------------------------------------------
double cardProbsTenUpAssumingNoBJ (int downCard)
{
  if (downCard == 1)
    return 0.; 
  else if (downCard == 10)
    return 4./12; //four tens/face cards 
  else 
    return 1./12; //each non-face card equally likely 
}



//------------------------------------------------------------------------------
// Returns the strategy one should use when splitting and doubling are allowed.
// handIndex is the index of the hand the player has, and upCard is the dealer's 
// up card. 
//------------------------------------------------------------------------------
Strategy splitOrDoubleStrat (Strategy **chart, int handIndex, int upCard) 
{
  double splitEV; 
  double ddWinProb, ddLossProb; 
  int splitCard; 
  Strategy strat = chart[handIndex][upCard]; 
  Hand hand = hands[handIndex]; 
	int **isSolved = iones(NUM_HANDS_SIMPLE, NUM_CARDS + 1); // needed by getSplitWinProb
  
  //First, determine whether to double. 
  ddWinProb = getDDWinProb (chart, hand, upCard); 
  ddLossProb = getDDLossProb (chart, hand, upCard); 
  
  //note: tie goes to not doubling to decrease variance 
  if (2. * (ddWinProb - ddLossProb) > strat.winPct - strat.lossPct) 
  {
    strat.action = DOUBLE_DOWN; 
    strat.winPct = ddWinProb; 
    strat.lossPct = ddLossProb; 
  }
  
  // Next, determine whether to split 
  if (hand.isSplittable)
  {
    splitCard = hand.isSoft ? 1 : hand.value / 2; 
    splitEV = getSplitEV (chart, splitCard, upCard); 
    
    //as with doubles, tie goes to not splitting 
    if (strat.action != DOUBLE_DOWN)
    {
      if (splitEV > strat.winPct - strat.lossPct)
      {
        strat.action = SPLIT; 
        strat.winPct = getSplitWinProb(chart, splitCard, upCard, isSolved); 
        strat.lossPct = getSplitLossProb(chart, splitCard, upCard, isSolved); 
        strat.splitEV = splitEV; 
      }
    }
    else //If current strat is to double down, multiply by 2 
    {
      if (splitEV > 2. * (strat.winPct - strat.lossPct)) 
      {
        strat.action = SPLIT; 
        strat.winPct = getSplitWinProb(chart, splitCard, upCard, isSolved); 
        strat.lossPct = getSplitLossProb(chart, splitCard, upCard, isSolved); 
        strat.splitEV = splitEV; 
      }
    }
  }

	freeimatrix (isSolved, NUM_HANDS_SIMPLE); 
  
  return strat; 
}



//------------------------------------------------------------------------------
// Computes the player's expected value and prints it to the terminal. E.g., if 
// the expected value is x, it means that the player will on average lose x 
// dollars on each hand when betting one dollar. 
//------------------------------------------------------------------------------
void computeExpectedValue (Strategy **chart, double BLACKJACK_PAYS)
{
  //expected value = probability of starting with each hand times expected 
  //value of each hand. 
  double *startingHandProbs = getStartingHandProbs(); 
  double *handExpVals = getHandExpVals (chart, BLACKJACK_PAYS); 
  
  //expected val
  double ev = dot (startingHandProbs, handExpVals, NUM_HANDS); 

  printf ("The player's expected value is %.3f%%. That is, a player betting "
      "$100 per hand will lose an average of $%.2f per hand.\n", 
      100. * ev, -100. * ev); 
  
  free (startingHandProbs); 
  free (handExpVals); 
}

//------------------------------------------------------------------------------
// Returns a vector of length NUM_HANDS whose ith entry is the probability that 
// the player will initially be dealt hand i. 
//------------------------------------------------------------------------------
double * getStartingHandProbs ()
{
  double *probs = NULL; 
  int i, j; 
  Hand hand; 
  int index; 
  double p; 
  
  probs = zerosv(NUM_HANDS); 
  if (probs == NULL) throwMemErr("probs", "getStartingHandProbs"); 
  
  //i and j run over all combinations of hands a player can be dealt 
  for (i = 1; i <= NUM_CARDS; i++)
  {
    for (j = 1; j <= NUM_CARDS; j++) 
    {
      hand = getHandByCards(i, j, FALSE); 
      index = getHandIndex(hand); 
      p = CARD_PROBABILITIES[i] * CARD_PROBABILITIES[j]; 
      probs[index] += p; 
    }
  }

  return probs; 
}

//------------------------------------------------------------------------------
// Returns a vector of length NUM_HANDS whose ith entry is the expected value 
// to the player of a hand in which his initial cards are given by hand i. 
//------------------------------------------------------------------------------
double * getHandExpVals (Strategy **chart, double BLACKJACK_PAYS)
{
  double *EVs = NULL; 
  int i; 
  
  //Probability that the dealer has blackjack 
  double probDealerBJ = 2. * CARD_PROBABILITIES[1] * CARD_PROBABILITIES[10]; 
  double EVDealerBJ, EVNoDealerBJ;  
  
  EVs = allocvector(NUM_HANDS); 
  if (EVs == NULL) throwMemErr("EVs", "getHandExpVals"); 
  
  for (i = 0; i < NUM_HANDS; i++)
  {
    //The expected value of each hand is the probability that dealer has 
    //blackjack times expected value given that dealer has blackjack, plus
    //probabilitity that dealer does not have blackjack times expected value
    //given that dealer does not have blackjack. 
    
    //If the player has blackjack it's a push, otherwise player loses his bet
    EVDealerBJ = (i == SOFT_TWENTYONE ? 0. : -1.); 
    EVNoDealerBJ = getEVOfHand (chart, i, BLACKJACK_PAYS); 
    
    EVs[i] = probDealerBJ * EVDealerBJ + (1. - probDealerBJ) * EVNoDealerBJ; 
  }
  
  return EVs; 
}

//------------------------------------------------------------------------------
// Returns the player's expected value when starting on the given hand, given
// that the dealer does not have blackjack. 
//------------------------------------------------------------------------------
double getEVOfHand (Strategy **chart, int handIndex, double BLACKJACK_PAYS)
{
  double EV = 0.; 
  double p; 
  int upCard; 
  Strategy strat; 
  
  if (handIndex == SOFT_TWENTYONE)
    return BLACKJACK_PAYS; 
  
  for (upCard = 1; upCard <= NUM_CARDS; upCard++)
  {
    strat = chart[handIndex][upCard]; 
    p = probOfUpCardGivenNoBJ(upCard); 

    if (strat.action == HIT || strat.action == STAND) 
      EV += p * (strat.winPct - strat.lossPct); 
    else if (strat.action == DOUBLE_DOWN) 
      EV += 2. * p * (strat.winPct - strat.lossPct); 
    else //if it's a split 
      EV += p * strat.splitEV; 
  }

  return EV; 
}


//------------------------------------------------------------------------------
// Returns the probability that the dealer has the following up card, 
// conditioned on the event that the dealer does not have blackjack. 
//------------------------------------------------------------------------------
double probOfUpCardGivenNoBJ (int upCard)
{
  //By Bayes' rule, this equals P(up card & no BJ) / P(no BJ). 
  double probBoth; 
  double probNoBJ = 1. - 2. * CARD_PROBABILITIES[1] * CARD_PROBABILITIES[10]; 
  
  if (upCard == 1)
    probBoth = CARD_PROBABILITIES[1] * (1. - CARD_PROBABILITIES[10]); 
  else if (upCard == 10)
    probBoth = CARD_PROBABILITIES[10] * (1. - CARD_PROBABILITIES[1]); 
  else 
    probBoth = CARD_PROBABILITIES[upCard]; 
  
  return probBoth / probNoBJ; 
}


// Returns the dot product of x and y, while ignoring values of y that are 
// undefined (signaled by negative values). Used by getHitWinProb among others.
double dot_ignore_undef (double *x, double *y, int N) { 
	int i; 
	double d = 0.; 

	for (i = 0; i < N; i++) { 
		if (y[i] >= 0.) {
			d += x[i] * y[i]; 
		}
		else if (x[i] > 0.) { 
			throwErr("attempt to multiply positive probability by undefined probability", "dot_ignore_undef"); 
		}
	}

	return d; 
}


/* Notes: 

1. Probability of winning and losing add up to less than 100% because a push may
  be possible. 
2. If calculateSimpleChart doesn't converge, don't calculate splits and doubles, 
  because DD is used as the code for strategies that were not solved. 
3. The reason both of these loops go "backwards" is because it is more likely 
  that a solution can be found first for "higher" hands and higher cards drawn.
*/



