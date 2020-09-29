#include <stdio.h>
#include <stdlib.h>    // termios stuff
#include <termios.h>    // termios stuff
#include <sys/ioctl.h> // for STDIN_FILENO
#include <unistd.h>    // ioctl()
//#include <signal.h>    // for signal, to catch the window size change (unused)
// https://man7.org/tlpi/code/online/dist/tty/demo_SIGWINCH.c.html

//#include "termstuff.h"

int tcols=80, trows=24;
/* If you want room for a status line...
   Set to at least 2 for now.  This is because I don't feel like
   handling the halving of the screen properly
   You can set this in your caller, at any time, and any new draws
   will use it in the calculations.
*/
int statuslines=0;

void cls() { printf("\033[2J"); }
void gotoxy(int x, int y) {
	printf("\033[%d;%dH", y, x);
}
void pat(char c, int x, int y) {
	printf("\033[%d;%dH%c", y, x, c);
}

// 0 is bottom status row, 1 is next up.
void gotostatus(int i) { gotoxy(1, trows-i); }

void cgotoxy(float x, float y) { // 0,0 is middle, 1 is up, -1 is left
	int cx = (tcols)/2, cy=(trows-statuslines)/2;
	gotoxy(cx + x*cx + 1, cy - y*cy +1);
}
void cpat(char c, float x, float y) { // 0,0 is middle, 1 is up, -1 is left
	cgotoxy(x, y);
	fputc(c, stdout);
}
void draw_axii() {
	for (float x=-1; x<=1; x+=.01) {
		cpat('-', x, 0);
		cpat('|', 0, x);
	}
	cpat('+', 0, 0);
}

void gettermsize() {
	#ifdef TIOCGSIZE
		struct ttysize ts;
		ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
		tcols = ts.ts_cols;
		trows = ts.ts_lines;
	#elif defined(TIOCGWINSZ)
		struct winsize ts;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
		tcols = ts.ws_col;
		trows = ts.ws_row;
	#endif /* TIOCGSIZE */
	//printf("Terminal is %dx%d\n", tcols, trows);
}

/* Initialize new terminal i/o settings */
static struct termios old;
static struct termios new1;
void term_icanon(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

void term_reset(void) { /* Restore old terminal i/o settings */
    tcsetattr(0, TCSANOW, &old);
}

void term_init() {
	setbuf(stdout, NULL); // unbuffy
	gettermsize();
	term_icanon(0);
	atexit(term_reset);
}
