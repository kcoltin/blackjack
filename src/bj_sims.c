#include "bj_sims.h" 
#include <stdlib.h> 
#include <math.h> 
#include "util/boolean.h" 
#include "util/error.h"
#include "util/linal.h" 
#include "util/moremath.h"
#include "util/stp.h"
#include "bj_strat.h" 
#include "hands.h" 


//------------------------------------------------------------------------------
// Runs N simulations of the player's hand i and dealer's up card. 
//------------------------------------------------------------------------------
void runSims(HandSim **simsChart, Strategy **chart, int i, int upCard, int N)
{
  int n; 
  int j; 
  int playerTotal, dealerTotal; 
  int index, splitCard, newCard; 
  int action; 
  Hand hand, dHand; 
  int isInitialHand; //true if it's the two cards first dealt - i.e. if 
                   //the player can split or double 
  
  int *deck; 
  int numDecks; 
  const int CARDS_PER_DECK = 52; //cards per deck 
  const int NUM_EACH_CARD = 4; //number of each card in a deck 
  int cardsInDeck; 
  int downCard; 
  
  n = 0; 
  while (n < N)
  {
    numDecks = 6;  
    cardsInDeck = numDecks * CARDS_PER_DECK; //number of cards remaining 
                                           //in the stack of decks 

    deck = NULL; 
    deck = (int *) malloc ((NUM_CARDS+1) * sizeof(int)); 
    if (deck == NULL) throwMemErr("deck", "runSims"); 
    for (j = 1; j <= NUM_CARDS - 1; j++)
      deck[j] = NUM_EACH_CARD * numDecks; 
    deck[10] = 4 * NUM_EACH_CARD * numDecks; //10 plus three face cards 
    removeCardsInStartingHand(deck, hands[i], cardsInDeck); 
    cardsInDeck -= 2; 
    
    //remove dealer's up card 
    deck[upCard]--; 
    //Draw dealer's down card. We're assuming it's not blackjack, so keep 
    //drawing until it's not.  
    do
    {
      downCard = randdraw_count2 (deck + 1, NUM_CARDS, cardsInDeck);
    } 
    while ((upCard == 1 && downCard == 10) || (upCard == 10 && downCard == 1)); 
    deck[downCard]--; 
    cardsInDeck -= 2; 
    
    //keep hitting to get final hand 
    index = i; //index of current hand
    hand = hands[i]; 
    isInitialHand = TRUE; 

    while (chart[index][upCard].action != STAND)
    {
      action = chart[index][upCard].action; 

      //If it's not the initial hand, convert to a non-splittable hand and/or
      //hit or stand instead of splitting or doubling down. 
      if (!(isInitialHand))
      {
        //if it's a double 3 through 10, convert to non-splittable form 
        if (hand.isSplittable && index >= THREES) 
        {
          hand.isSplittable = FALSE; 
          index = getHandIndex(hand);
          action = chart[index][upCard].action; 
        }
        
        if (action == DOUBLE_DOWN)
        {
          action = hand.value >= 18 ? STAND : HIT; 
          if (action == STAND)
            break; 
        }
      }
      
      if (action == HIT || action == DOUBLE_DOWN)
      {
        //if it's a double 3 through 10, convert to non-splittable form 
        if (hand.isSplittable && index >= THREES) 
        {
          hand.isSplittable = FALSE; 
          index = getHandIndex(hand);
        }
        
        //Draw a card  
        newCard = randdraw_count2 (deck + 1, NUM_CARDS, cardsInDeck); 
        deck[newCard]--;
        cardsInDeck--; 
        hand = calculateNewHand(hand, newCard); 
        index = getHandIndex(hand); 
        
        isInitialHand = FALSE; //after the first time through, it's not the
                        //initial hand anymore 
    
        if (action == DOUBLE_DOWN) //cannot hit further 
          break; 
      }
      else //if action = split. If "stand", we wouldn't be in the while loop.
      {
        //Compute the probability of winning each of the newly split hands;
        //ignores the case where the same card is dealt a third time causing
        //the hand to be resplit. 
        splitCard = hand.isSoft ? 1 : hand.value / 2; 
        do
        {
          newCard = randdraw_count2 (deck + 1, NUM_CARDS, cardsInDeck); 
        }
        while (newCard == splitCard); 
        deck[newCard]--; 
        cardsInDeck--; 
        
        hand = getHandByCards(splitCard, newCard, TRUE); //Note 1
        index = getHandIndex(hand); 
        
        isInitialHand = TRUE; //should already be true at this point; just 
                      //making sure. 
      }
    }

    playerTotal = hand.value; 
    
    //have dealer hit until standing 
    dHand = getHandByCards (upCard, downCard, TRUE); 
    while (!(doesDealerStand(dHand)))
    {
      newCard = randdraw_count2 (deck + 1, NUM_CARDS, cardsInDeck); 
      deck[newCard]--; 
      cardsInDeck--; 
      dHand = calculateNewHand (dHand, newCard); 
    }    
    dealerTotal = dHand.value; 

    if (doesPlayerWin(playerTotal, dealerTotal))
      simsChart[i][upCard].nwins++; 
    else if (doesPlayerLose(playerTotal, dealerTotal))
      simsChart[i][upCard].nlosses++; 
    
    n++; 
    free(deck); 
  }
  
}



//------------------------------------------------------------------------------
// Indicates whether the player wins given the indicated ending totals. 
//------------------------------------------------------------------------------
int doesPlayerWin (int playerTotal, int dealerTotal)
{
  if (playerTotal < BUST_VALUE 
    && (dealerTotal >= BUST_VALUE || playerTotal > dealerTotal))
    return TRUE; 
  else
    return FALSE;   
}


//------------------------------------------------------------------------------
// Indicates whether the player loses given the indicated ending totals. 
//------------------------------------------------------------------------------
int doesPlayerLose (int playerTotal, int dealerTotal)
{
  if (playerTotal >= BUST_VALUE 
    || (dealerTotal < BUST_VALUE && dealerTotal > playerTotal))
    return TRUE; 
  else
    return FALSE; 
}


//------------------------------------------------------------------------------
// Initializes a new chart of HandSims. 
//------------------------------------------------------------------------------
HandSim ** initializeSimsChart ()
{
  HandSim **simsChart = NULL; 
  int i, j; 
  
  simsChart = (HandSim **) malloc(NUM_HANDS * sizeof(HandSim *)); 
  for (i = 0; i < NUM_HANDS; i++)
    simsChart[i] = (HandSim *) malloc((NUM_CARDS+1) * sizeof(HandSim)); 
  if (simsChart == NULL) throwMemErr("simsChart", "initializeSimsChart"); 
  
  for (i = 0; i < NUM_HANDS; i++)
    for (j = 1; j <= NUM_CARDS; j++)
      simsChart[i][j] = newHandSim(); 
  
  return simsChart; 
}

//------------------------------------------------------------------------------
// Initializes a new HandSim 
//------------------------------------------------------------------------------
HandSim newHandSim ()
{
  HandSim hs = {0, 0}; 
  return hs; 
}



//------------------------------------------------------------------------------
// Returns the maximum difference between the theoretical and actual (from 
// simulations) percent chance of winning, over all hands and up cards. 
// Note: This ignores obvious hands, since they are not simulated. 
//------------------------------------------------------------------------------
double getMaxWinErr(Strategy **chart, HandSim **simsChart, int nsims)
{
  int i, j; 
  double mx = 0.; 
  double winPctActual, err; 
  
  for (i = 0; i < NUM_HANDS; i++)
  {
    if (hands[i].isObvious)
      continue; 
  
    for (j = 1; j <= NUM_CARDS; j++)
    {
      winPctActual = ((double) simsChart[i][j].nwins) / nsims; 
      err = fabs(chart[i][j].winPct - winPctActual); 
      if (err > mx)
        mx = err; 
    }
  }
        
  return 100. * mx; 
}


//------------------------------------------------------------------------------
// Returns the maximum difference between the theoretical and actual (from 
// simulations) percent chance of losing, over all hands and up cards. 
// Note: This ignores obvious hands, since they are not simulated. 
//------------------------------------------------------------------------------
double getMaxLossErr(Strategy **chart, HandSim **simsChart, int nsims)
{
  int i, j; 
  double mn = 0.; 
  double lossPctActual, err; 
  
  for (i = 0; i < NUM_HANDS; i++)
  {
    if (hands[i].isObvious)
      continue; 
  
    for (j = 1; j <= NUM_CARDS; j++)
    {
      lossPctActual = ((double) simsChart[i][j].nlosses) / nsims; 
      err = fabs(chart[i][j].lossPct - lossPctActual); 
      if (err > mn)
        mn = err; 
    }
  }
      
  return 100. * mn; 
}


//------------------------------------------------------------------------------
// Removes the cards in the player's starting hand from the deck. 
//------------------------------------------------------------------------------
void removeCardsInStartingHand (int *deck, Hand hand, int cardsInDeck)
{
  const int ACE_VALUE = 11; 
  int *cards; 
  int splitCard, otherCard; 

  //If it's a pair, remove the two cards. 
  if (hand.isSplittable)
  {
    splitCard = hand.isSoft ? 1 : hand.value / 2; 
    deck[splitCard] -= 2; 
  }
  //If it contains an ace, remove the two. 
  else if (hand.isSoft) 
  {
    deck[1]--; 
    otherCard = hand.value - ACE_VALUE; 
    deck[otherCard]--;     
  }
  else //May be hard 5 through 19. 
  {
    cards = chooseCardsInStartingHand (deck, hand.value, cardsInDeck); 
    deck[cards[0]]--; 
    deck[cards[1]]--; 
    free(cards); 
  }
}



//------------------------------------------------------------------------------
// Returns the two cars that comprise the player's starting hand, when the hand
// is neither a soft hand nor a splittable one. That is, it may be between hard
// 5 and hard 19. For some hands, there is only one possiblity. For most, 
// though, this randomly returns one of the possible pairs of cards that make up
// the hand, based on their respective probabilities of being drawn from the 
// given deck. "value" is the value of the (hard) hand. 
//
// Possible combos: 
// 5: 2/3 
// 6: 2/4  
// 7: 2/5, 3/4 
// 8: 2/6, 3/5 
// 9: 2/7, 3/6, 4/5 
// 10: 2/8, 3/7, 4/6 
// 11: 2/9, 3/8, 4/7, 5/6 
// 12: 2/10, 3/9, 4/8, 5/7 
// 13: 3/10, 4/9, 5/8, 6/7 
// 14: 4/10, 5/9, 6/8 
// 15: 5/10, 6/9, 7/8 
// 16: 6/10, 7/9 
// 17: 7/10, 8/9 
// 18: 8/10 
// 19: 9/10 
//  
//------------------------------------------------------------------------------
int * chooseCardsInStartingHand (int *deck, int value, int cardsInDeck)
{
  int *cards = NULL; 
  double *probs = NULL; 
  
  //These vectors give the combinations of possible pairs of cards that can 
  //produce the given hand. The two cards lesserCards[i] and greaterCards[i]
  //will add up to "value", and will be selected with probability probs[i]. 
  int *lesserCards = NULL; 
  int *greaterCards = NULL; 
  
  const int MAX_COMBOS = 4; //maximum number of combinations of two cards that
                  //can make a given hand 
  int i, j, count; 
  double sum; 
  
  cards = (int *) malloc (2 * sizeof(int)); 
  if (cards == NULL) throwMemErr("cards", "chooseCardsInStartingHand"); 
  lesserCards = (int *) malloc (MAX_COMBOS * sizeof(int)); 
  if (lesserCards == NULL) 
    throwMemErr("lesserCards", "chooseCardsInStartingHand"); 
  greaterCards = (int *) malloc (MAX_COMBOS * sizeof(int)); 
  if (greaterCards == NULL) 
    throwMemErr("greaterCards", "chooseCardsInStartingHand"); 
  probs = allocvector(MAX_COMBOS); 
  if (probs == NULL) throwMemErr("probs", "chooseCardsInStartingHand"); 
  
  count = 0; 
  for (i = maxi(2, value - 10); i < value / 2.; i++)
  {
    j = value - i; 
    lesserCards[count] = i; 
    greaterCards[count] = j; 
    probs[count] = 2. * ((double) deck[i]) * ((double) deck[j])
              / (cardsInDeck * cardsInDeck); 
    count++; 
  }
  
  sum = vectorsum(probs, count); 
  for (i = 0; i < count; i++)
    probs[i] /= sum; 
  
  i = randdraw(probs, count) - 1; 
  cards[0] = lesserCards[i]; 
  cards[1] = greaterCards[i]; 
  
  free(lesserCards); 
  free(greaterCards); 

  return cards; 
}


/* NOTES 

1. "isDealer" may be set either true or false; it won't make a difference since 
  the new card will never equal the split card. 
*/ 









