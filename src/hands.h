/*
 *  hands.h
 *  Kevin Coltin 
 * 
 *  Contains a list of all possible Blackjack hands and functions to initialize
 *  them. 
 */


#ifndef HANDS_H 
#define HANDS_H 

//Represents a hand, in terms of the cards held 
typedef struct {
  int value; //point value of cards (if possible, ace is counted as 11)
  int isSoft; //if true, it's a soft hand (i.e. an ace is being used as 11)
  int isObvious; //if true, it is obvious how to play the hand (e.g. 7 or less)
            //so it should be omitted from the printed chart. 
  int isSplittable; //true if it's double cards 
} Hand; 

//Number of relevant hands: hard 3-21, soft 12-21, doubles A-10, and bust.  
extern const int NUM_HANDS;
extern const int NUM_HANDS_SIMPLE; //number of hands ignoring doubles (includes 
                      //double aces though, since it's the same as
                      //two). 

extern const int BUST_VALUE; //indicates any value over 21 

//Vector of all possible hands. 
extern Hand *hands; 

//Constants to use to refer to each hand 
extern const int FOUR; //splittable; equals 22
extern const int FIVE; 
extern const int SIX; 
extern const int SEVEN; 
extern const int EIGHT; 
extern const int NINE; 
extern const int TEN; 
extern const int ELEVEN; 
extern const int TWELVE; 
extern const int THIRTEEN; 
extern const int FOURTEEN; 
extern const int FIFTEEN; 
extern const int SIXTEEN;
extern const int SEVENTEEN;
extern const int EIGHTEEN;
extern const int NINETEEN;
extern const int TWENTY;
extern const int TWENTYONE;
extern const int SOFT_TWELVE; //splittable; equals AA
extern const int SOFT_THIRTEEN; 
extern const int SOFT_FOURTEEN;
extern const int SOFT_FIFTEEN;
extern const int SOFT_SIXTEEN;
extern const int SOFT_SEVENTEEN;
extern const int SOFT_EIGHTEEN;
extern const int SOFT_NINETEEN;
extern const int SOFT_TWENTY;
extern const int SOFT_TWENTYONE;
extern const int BUST;
//NOTE: double aces is the same as soft twelve, and double twos is the same as 
//four. 
extern const int THREES;
extern const int FOURS;
extern const int FIVES;
extern const int SIXES; 
extern const int SEVENS; 
extern const int EIGHTS; 
extern const int NINES; 
extern const int TENS; 


void makeHands (); 
Hand makeHand (int, int, int, int); 
int getHandIndex (Hand); 
Hand getHand (Hand);
int areHandsEqual (Hand, Hand); 
char * getHandName (Hand); 
Hand getHandByCards (int, int, int);  

#endif
