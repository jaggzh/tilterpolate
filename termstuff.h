#ifndef TERMSTUFF_H
#define TERMSTUFF_H

void init(); // call me to set buf to nothing
void cls();  // clear screen
void gotoxy(int x, int y);      // 1,1 is top left
void pat(char c, int x, int y); // print at

// Centered cartesian stuff [-1,0,1]
void cgotoxy(float x, float y);      // 0,0 is middle, 1 is up, -1 is left
void cpat(char c, float x, float y); // print c at
void draw_axii();

// Term stuff
void term_icanon(int echo); // set raw input
void term_reset(void); // Restore old terminal i/o settings

#endif