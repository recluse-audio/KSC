/**
 * Made by Ryan Devens on 2026-02-24
 */


/**
 * Things you find that go into your notes/inventory and will reference later
 * 
 * This could be a password that character writes in notes,
 * or a key to a vehicle that belonged to a perp.
 * It is treated the same way, having the tool or the intel is the same.
 * 
 * This game is very linear, so the question is always boolean. Did they find the clue or not?
 * Challenge is going to be a bit esoteric as to where they need to click so that will add difficulty.
 * 
 * Clues will be discovered in the Locations. Which equates to - user clicks the clue's zone (as defined in location)
 * and sets the associated boolean to true.
 */
class Clues
{
public:

// Desk
    bool cABPasswordDiscovered = false;
    bool cLoginHistoryDiscovered = false;

};