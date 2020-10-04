#include <stdio.h>
#include <stdlib.h>    // termios stuff
#include <termios.h>    // termios stuff
#include <sys/ioctl.h> // for STDIN_FILENO
#include <unistd.h>    // ioctl()
#include <signal.h>    // for signal, to catch the window size change

#define FILE_SAVE_PAIRS "data/pairs.txt"
// rotating static buffers, per function, for color codes
#define TERM_STATIC_BUFFERS 10

// https://man7.org/tlpi/code/online/dist/tty/demo_SIGWINCH.c.html

#include "termstuff.h"

int term_flags=TERMF_DEFAULTS;

int tcols=80, trows=24;
/* If you want room for a status line...
   Set to at least 2 for now.  This is because I don't feel like
   handling the halving of the screen properly
   You can set this in your caller, at any time, and any new draws
   will use it in the calculations.
*/
int statuslines=0;

void cls() { printf("\033[2J"); }
void rst() { printf("\033[0m"); }
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
	for (float x=-1; x<=1; x+=.005) {
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

void sigwinch_handler(int sig) {
	gettermsize();
	if (term_flags & TERMF_CLS_ON_WINCH) cls();
}

void set_winch() {
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigwinch_handler;
	if (sigaction(SIGWINCH, &sa, NULL) == -1) {
		perror("sigaction() from termstuff.c");
		exit(1);
	}
}

char *rgb24bg_f(float r, float g, float b) {
	static char code[TERM_STATIC_BUFFERS][23];
	static int bi=0;
	// pre-increment instead of post. we end up starting at 1 but .. whatever
	bi = bi<TERM_STATIC_BUFFERS-1 ? bi+1 : 0;
	if (r<0) r=0;
	if (g<0) g=0;
	if (b<0) b=0;             // chop little vals
	if (r>1.0) r=1.0;
	if (g>1.0) g=1.0;
	if (b>1.0) b=1.0; // chop big vals
	sprintf(code[bi], "\033[48;2;%d;%d;%dm", int(r*255), int(g*255), int(b*255));
	return code[bi];
}

void term_init_with_flags(int flags) {  term_flags=flags;  term_init();  }
void term_enable_flag(int flag) { term_flags |= flag; }
void term_disable_flag(int flag) { term_flags = term_flags & ~flag; }

void term_init() {
	setbuf(stdout, NULL); // unbuffy
	gettermsize();
	term_icanon(0);
	atexit(term_reset);
	if (term_flags & TERMF_WINCH) set_winch();
}
