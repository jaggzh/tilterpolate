#ifndef TERMSTUFF_H
#define TERMSTUFF_H

// Usage: Call term_init().
// By default It will disable output buffering and enable unbuffered input
// (and no echo).
// It also sets an atexit() to restore it. Good luck.

extern int statuslines;
extern int tcols, trows;

// FLAGS.  Ones marked LIVE can be set any time
#define TERMF_WINCH          1  // Catch window change size
#define TERMF_CLS_ON_WINCH   2  // [LIVE] cls() on window size change
#define TERMF_DEFAULTS (TERMF_WINCH)

void gotostatus(int i); // 0 is bottom status row, 1 is next up.

void term_init(); // call me!  Will disable output buffering
void term_init_with_flags(int flags);
void term_enable_flag(int flag);
void term_disable_flag(int flag);

void cls();  // clear screen
void rst();  // clear colors

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
