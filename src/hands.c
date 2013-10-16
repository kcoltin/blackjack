#include "hands.h"
#include <stdio.h> 
#include <stdlib.h> 
#include "util/boolean.h"
#include "util/error.h"
#include "bj_strat.h"

Hand *hands; 

const int NUM_HANDS = 37;
const int NUM_HANDS_SIMPLE = 29; 
const int BUST_VALUE = 22; 

const int FOUR = 0; 
const int FIVE = 1; 
const int SIX = 2; 
const int SEVEN = 3; 
const int EIGHT = 4; 
const int NINE = 5; 
const int TEN = 6; 
const int ELEVEN = 7; 
const int TWELVE = 8; 
const int THIRTEEN = 9; 
const int FOURTEEN = 10; 
const int FIFTEEN = 11; 
const int SIXTEEN = 12; 
const int SEVENTEEN = 13; 
const int EIGHTEEN = 14; 
const int NINETEEN = 15; 
const int TWENTY = 16; 
const int TWENTYONE = 17; 
const int SOFT_TWELVE = 18; 
const int SOFT_THIRTEEN = 19; 
const int SOFT_FOURTEEN = 20; 
const int SOFT_FIFTEEN = 21; 
const int SOFT_SIXTEEN = 22; 
const int SOFT_SEVENTEEN = 23; 
const int SOFT_EIGHTEEN = 24; 
const int SOFT_NINETEEN = 25; 
const int SOFT_TWENTY = 26; 
const int SOFT_TWENTYONE = 27; 
const int BUST = 28; 
const int THREES = 29; 
const int FOURS = 30; 
const int FIVES = 31; 
const int SIXES = 32; 
const int SEVENS = 33; 
const int EIGHTS = 34; 
const int NINES = 35; 
const int TENS = 36; 


//------------------------------------------------------------------------------
// Makes a vector containing every possible hand. Each hand is indexed by its 
// name as the constants listed above. Note that doubles are listed last, so 
// that the beginning of the vector can be used as a vector of "simple" hands. 
//------------------------------------------------------------------------------
void makeHands ()
{
  int i; 
  int offset; 

  hands = NULL; 
  hands = (Hand *) malloc(NUM_HANDS * sizeof(Hand)); 
  if (hands == NULL) throwMemErr("hands", "makeHands"); 
  
  offset = 4; //difference between value and index of cards, for FOUR 
            //through TWENTYONE 
  for (i = FOUR; i <= SEVEN; i++)
    hands[i] = makeHand(i + offset, FALSE, TRUE, FALSE); 
  for (i = EIGHT; i <= SEVENTEEN; i++)
    hands[i] = makeHand(i + offset, FALSE, FALSE, FALSE); 
  for (i = EIGHTEEN; i <= TWENTYONE; i++)
    hands[i] = makeHand(i + offset, FALSE, TRUE, FALSE); 
  
  //Print four, because it's 2,2
  hands[FOUR].isObvious = FALSE; 
  
  offset = -6;  
  for (i = SOFT_TWELVE; i <= SOFT_TWENTYONE; i++)
    hands[i] = makeHand(i + offset, TRUE, FALSE, FALSE); 
  
  hands[SOFT_TWENTY].isObvious = TRUE; 
  hands[SOFT_TWENTYONE].isObvious = TRUE; 
  
  hands[BUST] = makeHand(BUST_VALUE, FALSE, TRUE, FALSE); 
  
  //Four and soft twelve are splittable 
  hands[FOUR].isSplittable = TRUE; 
  hands[SOFT_TWELVE].isSplittable = TRUE; 
  
  offset = -26; 
  for (i = THREES; i <= TENS; i++)
    hands[i] = makeHand(2 * (i + offset), FALSE, FALSE, TRUE); 
}


//------------------------------------------------------------------------------
// Constructor for Hand struct 
//------------------------------------------------------------------------------
Hand makeHand (int value, int isSoft, int isObvious, int isSplittable)
{
  Hand hand = {value, isSoft, isObvious, isSplittable}; 
  return hand;   
}

//------------------------------------------------------------------------------
// Returns the index number (THREE, FOUR, etc.) of a given hand. hands should 
// be the vector of all hands created by makeHands(). 
//------------------------------------------------------------------------------
int getHandIndex (Hand hand)
{
  int i = 0; 
  while (i < NUM_HANDS)
  {
    if (areHandsEqual(hand, hands[i]))
      return i; 
    i++;
  }
  
  throwErr("Hand is not equal to any existing hand.", "getHandIndex"); 
  return -1; 
}


//------------------------------------------------------------------------------
// Returns the hand in the hands array that is equal to the given hand. 
//------------------------------------------------------------------------------
Hand getHand (Hand hand)
{
  int i = 0; 
  while (i < NUM_HANDS)
  {
    if (areHandsEqual(hand, hands[i]))
      return hands[i]; 
    i++;
  }
  
  throwErr("Hand is not equal to any existing hand.", "getHand"); 
  return hands[0]; 
}


//------------------------------------------------------------------------------
// Indicates whether two hands are the same hand. Note: This does not check 
// whether they are obvious, because it is not logically necessary here and it 
// makes other functions such as calculateNewHand simpler. 
//------------------------------------------------------------------------------
int areHandsEqual (Hand hand1, Hand hand2)
{
  return hand1.value == hand2.value 
      && hand1.isSoft == hand2.isSoft 
      && hand1.isSplittable == hand2.isSplittable; 
}



//------------------------------------------------------------------------------
// Returns the symbolic "name" of the hand: e.g. 14, A,7, 8,8. 
//------------------------------------------------------------------------------
char * getHandName (Hand hand)
{
  char *name = NULL; 
  const int ACE_VALUE = 11; 
  const int MAX_LENGTH = 6; //"10,10" + null termination character
  
  name = (char *) malloc(MAX_LENGTH*sizeof(char)); 
  if (name == NULL) throwMemErr("name", "getHandName"); 
  
  if (hand.isSplittable)
  {
    if (hand.isSoft)
      sprintf(name, "A,A"); 
    else
      sprintf(name, "%d,%d", hand.value / 2, hand.value / 2); 
  }
  else if (hand.isSoft)
    sprintf(name, "A,%d", hand.value - ACE_VALUE); 
  else if (hand.value == BUST_VALUE)
    sprintf(name, "XXX"); 
  else 
    sprintf(name, "%d", hand.value); 
  
  return name;   
}


//------------------------------------------------------------------------------
// Gives the hand that results from the given two cards. 
// If isDealer is true, the function ignores splits since they don't apply to 
// the dealer.  isDealer can also be set true if one wants to ignore the 
// possbility of splits for other reasons.
//------------------------------------------------------------------------------
Hand getHandByCards (int card1, int card2, int isDealer)
{
  int isEitherAce = card1 == 1 || card2 == 1; 
  Hand hand = {card1 + card2 + 10*isEitherAce,
          isEitherAce, 
          FALSE, 
          card1 == card2 && card1 <= 2}; //Note 1
  
  if (card1 == card2 && !(isDealer))
    hand.isSplittable = TRUE; 
  
  return getHand(hand); 
}


/* NOTES

1. If card1 and card2 are both A or 2, then the hand has to be splittable (because 
   four and soft 12 are defined as splittable). This is necessary so that the hand 
  will match up with the correct hand in the hands array. 

*/ 





