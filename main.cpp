#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h> // sleep
#include "termstuff.h"
#include "radterpolate.h"

//#define SHOW_FS_FIELD // show full screen field? (uncomment for no)

#define SENSOR_MULT 1 // Scale values to match what your sensor might output
#define MOVEINC 10*SENSOR_MULT // When moving in the test vector field

// Extending radterpolate's types T_LEFT, etc...
#define OT_POINT  (POINTS+0) // Our Type
#define OT_CENTER (POINTS+1)
#define OT_CNT    (POINTS+2)

#define PTLEFT   ptpairs[T_LEFT]
#define PTRIGHT  ptpairs[T_RIGHT]
#define PTUP     ptpairs[T_UP]
#define PTDOWN   ptpairs[T_DOWN]
#define PTCENTER ptpairs[OT_CENTER]
#define PTPOINT  ptpairs[OT_POINT]

#define PSYMLEFT   pt_syms[T_LEFT]
#define PSYMRIGHT  pt_syms[T_RIGHT]
#define PSYMUP     pt_syms[T_UP]
#define PSYMDOWN   pt_syms[T_DOWN]
#define PSYMCENTER pt_syms[OT_CENTER]
#define PSYMPOINT  pt_syms[OT_POINT]

//#define CGOTOPOINT(p) cgotoxy(p.x/mr, p.y/mr)
#define CGOTOPOINT(p) gotorangedxy(p.x+test_offsetx, p.y+test_offsety, mins, maxs)

#define PX (ptpairs[OT_POINT].x)
#define PY (ptpairs[OT_POINT].y)

const char pt_keys[OT_CNT+1] = {
	'L', 'R', 'U', 'D', '.', ',', 0
};
const char *pt_syms[OT_CNT+1] = {
	"Left", "Right", "Up", "Down", "*", "(C)", NULL
};
const char *pt_humanlabels[OT_CNT+1] = {
	"Left", "Right", "Up", "Down", "User Point", "Center", NULL
};

void update_radterp_system(Radterpolator *mousep, fPair *ptpairs) {
	printf("\n\033[41;37;1mSetting left %f\033[0m\n", ptpairs[T_LEFT].x);
	printf("\033[41;37;1mSetting right %f\033[0m\n", ptpairs[T_RIGHT].x);
	mousep->set_left(   ptpairs[T_LEFT]   );
	mousep->set_right(  ptpairs[T_RIGHT]  );
	mousep->set_down(   ptpairs[T_DOWN]   );
	mousep->set_up(     ptpairs[T_UP]     );
	mousep->set_center( ptpairs[OT_CENTER] );
	mousep->prep();
}

void gotorangedxy(float x, float y, fPair mins, fPair maxs) {
	float nx, ny;
	nx = ((x - mins.x) / (maxs.x - mins.x))*2 - 1;
	ny = ((y - mins.y) / (maxs.y - mins.y))*2 - 1;
	cgotoxy(nx, ny);
}

// mr is the max range of values
void plot_field(Radterpolator *mousep, bool showx, bool showy, fPair ranges, fPair mins, fPair maxs) {
	#define FIELDX_STEP 37
	#define FIELDY_STEP 7
	#define FIELD_LBL_CHARS 6   // 4 chars + space
	int lblchars=1;
	if (showx) lblchars += 2;
	if (showy) lblchars += 2;
	float xcnt = ((float)tcols) / lblchars;
	//float xrange = mr + mr*.9;
	float xrange = maxs.x - mins.x;
	float yrange = maxs.y - mins.y;
	float xstep = xrange / (xcnt);
	float ystep = yrange / ((((float)trows - statuslines)));
	//printf("xstep: %d\n", (int)(xstep));
	//printf("ystep: %d\n", (int)(ystep));
	for (float y=mins.y; y<=maxs.y; y += ystep) {
		for (float x=mins.x; x<maxs.x - xstep; x += xstep) {
			fPair res;
			res = mousep->interp(x, y);
			//cgotoxy(x/mr, y/mr);
			gotorangedxy(x, y, mins, maxs);

			const char *bgx, *bgy;
			bgx=bgy="\033[40m";
			if (res.x < -1.0)      bgx=rgb24bg_f( .8, .3, .0); 
			else if (res.x > 1.0)  bgx=rgb24bg_f( .8, .0, .4); 
			else {                 bgx=rgb24bg_f( abs(res.x)*.8 , 0, 0); }
			if (res.y < -1.0)      bgy=rgb24bg_f( .3, .8, .0); 
			else if (res.y > 1.0)  bgy=rgb24bg_f( .0, .8, .4); 
			else {                 bgy=rgb24bg_f(  0, abs(res.y)*.8, 0); }
			if (showx) printf("%s\033[37m%2d", bgx, (int)(fminf(res.x,.9)*10));
			if (showy) printf("%s\033[32m%2d", bgy, (int)(fminf(res.y,.9)*10));
			printf(" ");
			/* printf("\n"); */
		}
		printf("\033[0m\033[K");
	}
}

// sets x and y ranges from ptpairs list
void set_ranges(fPair *rangesp, fPair *minsp, fPair *maxsp, fPair *ptpairs) {
	for (int i=1; i<OT_CNT; i++) {
		if (ptpairs[i].x < minsp->x) minsp->x = ptpairs[i].x - (maxsp->x - minsp->x)*.05;
		if (ptpairs[i].x > maxsp->x) maxsp->x = ptpairs[i].x + (maxsp->x - minsp->x)*.05;
		if (ptpairs[i].y < minsp->y) minsp->y = ptpairs[i].y - (maxsp->y - minsp->y)*.05;
		if (ptpairs[i].y > maxsp->y) maxsp->y = ptpairs[i].y + (maxsp->y - minsp->y)*.05;
	}
	rangesp->x = maxsp->x - minsp->x;
	rangesp->y = maxsp->y - minsp->y;
}

void set_ranges_init(fPair *rangesp, fPair *minsp, fPair *maxsp, fPair *ptpairs) {
	minsp->x = ptpairs[0].x-.001;
	maxsp->x = ptpairs[0].x+.001;
	minsp->y = ptpairs[0].y-.001;
	maxsp->y = ptpairs[0].y+.001;
	set_ranges(rangesp, minsp, maxsp, ptpairs);
}

void print_ranges(fPair mins, fPair maxs) {
	printf("[%.1f < x < %.1f] ", mins.x, maxs.x);
	printf("[%.1f < y < %.1f]", mins.y, maxs.y);
}

int main(int argc, char *argv[]) {
	int cur_pointi;
	bool showx=true, showy=false;
	bool auto_range=true;
	fPair ranges;
	fPair mins, maxs;
	Radterpolator mouse;
	fPair ptpairs[OT_CNT+1]; // theirs plus our Point and Center
	fPair tmpptpairs[OT_CNT+1]; // theirs plus our Point and Center
	#ifdef SHOW_FS_FIELD
		//float mr=310; // max range (to scale for plotting)
		//mr *= SENSOR_MULT;
	#endif

	statuslines=3;
	// We init the term even if we're not showing the vector'ish field
	// (ie. even if SHOW_FS_FIELD is not defined), because we still take
	// input... so we can take user menu selections
	term_init_with_flags(TERMF_DEFAULTS | TERMF_CLS_ON_WINCH);


	// Normal'ish person test set
	ptpairs[T_LEFT].set(-250, 55);
	ptpairs[T_RIGHT].set(250, -50);
	ptpairs[T_DOWN].set(30, -220);
	ptpairs[T_UP].set(-20, 200);
	ptpairs[OT_CENTER].set(40, -60);
	ptpairs[OT_POINT].set(-101, -59);

	// Real life values from rhino mouse
	ptpairs[T_LEFT].set(8000, -10600);
	ptpairs[T_RIGHT].set(10100, -4200);
	ptpairs[T_DOWN].set(11300, -4700);
	ptpairs[T_UP].set(11300, -7300);
	ptpairs[OT_CENTER].set(10000, -7700);
	ptpairs[OT_POINT].set(11000, -7200);

	// Example disabled use: L,R,Up,Center are normal'ish. Down is near Up
	ptpairs[T_LEFT].set(-250*SENSOR_MULT, 55*SENSOR_MULT);
	ptpairs[T_RIGHT].set(250*SENSOR_MULT, 30*SENSOR_MULT);
	ptpairs[T_DOWN].set(-70*SENSOR_MULT, 170*SENSOR_MULT);
	ptpairs[T_UP].set(-20*SENSOR_MULT, 200*SENSOR_MULT);
	ptpairs[OT_CENTER].set(30*SENSOR_MULT, 32*SENSOR_MULT);
	ptpairs[OT_POINT].set(200*SENSOR_MULT, -45*SENSOR_MULT);

	// OFFSET Example disabled use: L,R,Up,Center are normal'ish. Down is near Up
	float test_offsetx = 0;
	float test_offsety = 0;

	ptpairs[T_LEFT].set(-250, -55);
	ptpairs[T_RIGHT].set(250, -30);
	ptpairs[T_DOWN].set(-70, 170);
	ptpairs[T_UP].set(20, 200);
	ptpairs[OT_CENTER].set(30, -10);
	ptpairs[OT_POINT].set(-40, 195);


	// Real life values from rhino mouse
	ptpairs[T_LEFT].set(-8000, -11600);
	ptpairs[T_RIGHT].set(13100, 1800);
	ptpairs[T_DOWN].set(-12000, -7000);
	ptpairs[T_UP].set(11300, -3000);
	ptpairs[OT_CENTER].set(12000, -4500);
	ptpairs[OT_POINT].set(11000, -7200);

	// Real life values from rhino mouse using DMP and angles up to 180
	ptpairs[T_LEFT].set(-15, -6);
	ptpairs[T_RIGHT].set(12, 0);
	ptpairs[T_DOWN].set(0, 10);
	ptpairs[T_UP].set(0, -12);
	ptpairs[OT_CENTER].set(0, 0);
	ptpairs[OT_POINT].set(1,1);

	set_ranges_init(&ranges, &mins, &maxs, ptpairs);
	print_ranges(mins, maxs);
	update_radterp_system(&mouse, ptpairs);

	cur_pointi = T_LEFT;

	/*
	printf("RangeX: %.1f < x < %.1f\n", mins.x, maxs.x);
	printf("RangeY: %.1f < y < %.1f\n", mins.y, maxs.y);
	gotorangedxy(0,0,mins,maxs);
	printf("\033[41;37;1m*\033[0m\n");
	printf("Termsize: %d x %d\n", tcols, trows);
	sleep(3);
	exit(0);
	*/

	cls();
	test_offsetx=0;
	for (int i=0; i<OT_CNT; i++)
		tmpptpairs[i].set(ptpairs[i].x + test_offsetx, ptpairs[i].y + test_offsety);
	set_ranges(&ranges, &mins, &maxs, tmpptpairs);
	update_radterp_system(&mouse, tmpptpairs);
	while (1) {
		fPair res;
		#ifdef SHOW_FS_FIELD
			//cls();
			plot_field(&mouse, showx, showy, ranges, mins, maxs);
			draw_axii();
			gotostatus(2);
			printf("Center: %d,%d  Point: %d,%d  Offset: %d,%d ",
					(int)(PTCENTER.x+test_offsetx), (int)(PTCENTER.y+test_offsety),
					(int)(PTPOINT.x+test_offsetx), (int)(PTPOINT.y+test_offsety),
					(int)(test_offsetx), (int)(test_offsety)
					);
			print_ranges(mins, maxs);
			printf("  ");
			gotostatus(1);
		#endif
		printf("Currently moving: \033[33;1m%s\033[0m\033[K", pt_humanlabels[cur_pointi]);
		#ifdef SHOW_FS_FIELD
			gotostatus(0);
		#endif
		printf("(hjkl) move. (q)uit. Select (%c%c%c%c %c %c). Toggle (x(%s),y(%s)). (r)ange, (a)utorange(%s)\033[K",
			pt_keys[T_LEFT], pt_keys[T_RIGHT], pt_keys[T_UP], pt_keys[T_DOWN],
			pt_keys[OT_POINT], pt_keys[OT_CENTER],
			showx ? "\033[36;1mon\033[0m" : "\033[31moff\033[0m",
			showy ? "\033[36;1mon\033[0m" : "\033[31moff\033[0m",
			auto_range ? "\033[36;1mon\033[0m" : "\033[31moff\033[0m"
			);
		#ifdef SHOW_FS_FIELD
			for (int i=0; i<POINTS; i++) {
				//cgotoxy(ptpairs[i].x/mr, ptpairs[i].y/mr);
				gotorangedxy(ptpairs[i].x+test_offsetx, ptpairs[i].y+test_offsety, mins, maxs);
				printf("%s", pt_syms[i]);
			}
			CGOTOPOINT(PTLEFT); printf("\033[37;1;41m%s\033[0m", PSYMLEFT);
			CGOTOPOINT(PTRIGHT); printf("\033[37;1;41m%s\033[0m", PSYMRIGHT);
			CGOTOPOINT(PTUP); printf("\033[37;1;42m%s\033[0m", PSYMUP);
			CGOTOPOINT(PTDOWN); printf("\033[37;1;42m%s\033[0m", PSYMDOWN);
			CGOTOPOINT(PTCENTER); printf("\033[33;44;1m%s\033[0m", PSYMCENTER);
		#endif

		////////////////////////////
		// \/  Interpolate here
		res = mouse.interp(ptpairs[OT_POINT].x+test_offsetx, ptpairs[OT_POINT].y+test_offsety);
		////////////////////////////

		#ifdef SHOW_FS_FIELD
			CGOTOPOINT(PTPOINT);
			printf("\033[31;44;1m%.1f\033[1;30m,\033[32;44;1m%.1f\033[0m", res.x, res.y);
		#endif
		char c;
		bool pchange;
		pchange = false;
		if (read(0, &c, 1)) {
			if (c=='q') break;
			if (c=='h') ptpairs[cur_pointi].x -= ranges.x*.05, pchange=true;
			else if (c=='l') ptpairs[cur_pointi].x += ranges.x*.05, pchange=true;
			else if (c=='j') ptpairs[cur_pointi].y -= ranges.y*.05, pchange=true;
			else if (c=='k') ptpairs[cur_pointi].y += ranges.y*.05, pchange=true;
			else if (c=='x') showx = !showx;
			else if (c=='y') showy = !showy;
			else if (c=='r') set_ranges_init(&ranges, &mins, &maxs, ptpairs);
			else if (c=='a') auto_range = !auto_range;
			else if (c=='>') test_offsetx += 10, pchange=true;
			else if (c=='<') test_offsetx -= 10, pchange=true;
			//else if (c=='s') save_pairs(ptpairs); // maybe later
			else {
				for (int i=0; i<OT_CNT; i++) {
					if (c == pt_keys[i]) {
						cur_pointi = i;
					}
				}
			}
			for (int i=0; i<OT_CNT; i++) {
				tmpptpairs[i].set(ptpairs[i].x + test_offsetx, ptpairs[i].y + test_offsety);
			}
			if (pchange && auto_range) set_ranges_init(&ranges, &mins, &maxs, ptpairs);
			set_ranges(&ranges, &mins, &maxs, tmpptpairs);
			update_radterp_system(&mouse, tmpptpairs);
		}
		#ifndef SHOW_FS_FIELD
			printf("\n");
		#endif
	}
	return 0;
}

/* Video title and description:
Successful mouse interpolation of arbitrary user sensor positions
To assist users with disabilities (starting with one), I have to convert an arbitrary sensor space (gyroscope x and y "tilt" values) to the user's desired mouse cursor movements.  Due to such persons' limitations in movement, they may need to set a point representing, for example, "move up", anywhere in the tilting space of the mouse.  Then I must interpolate the x and y coordinates for final output mouse moves (max range of [-1,1]) in the x and y direction.  I will show the method of interpolation later.
*/

// vim: ts=4 sw=4
