#ifndef TERMSTUFF_H
#define TERMSTUFF_H

// Usage: Call init() to unbuffer output. Then do stuff.
// currently init() also sets input to unbuffered and no echo too...
// and sets an atexit() to restore it. Good luck.

extern int statuslines;
extern int tcols, trows;

void gotostatus(int i); // 0 is bottom status row, 1 is next up.

void term_init(); // call me to set buf to nothing
void cls();  // clear screen

// Standard 1-cols, 1-rows ANSI stuff
void gotoxy(int x, int y);      // 1,1 is top left
void pat(char c, int x, int y); // print at

// Centered cartesian stuff [-1,0,1]
void cgotoxy(float x, float y);      // 0,0 is middle, 1 is up, -1 is left
void cpat(char c, float x, float y); // print c at
void draw_axii();

// Term stuff
void term_icanon(int echo); // set raw input
void term_reset(void); // Restore old terminal i/o settings

// Colors
char *rgb24bg_f(float r, float g, float b); // 0-1.0 -> 24bit *color code string

#endif
