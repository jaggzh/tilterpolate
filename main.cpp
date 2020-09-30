#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h> // sleep
#include "termstuff.h"
#include "radterpolate.h"

#define SHOW_FS_FIELD // show full screen field? (uncomment for no)

#define MOVEINC 10

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

#define CGOTOPOINT(p) cgotoxy(p.x/mr, p.y/mr)

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
	mousep->set_left(   ptpairs[T_LEFT]   );
	mousep->set_right(  ptpairs[T_RIGHT]  );
	mousep->set_down(   ptpairs[T_DOWN]   );
	mousep->set_up(     ptpairs[T_UP]     );
	mousep->set_center( ptpairs[OT_CENTER] );
	mousep->prep();
}

// mr is the max range of values
void plot_field(Radterpolator *mousep, float mr, bool showx, bool showy) {
	#define FIELDX_STEP 37
	#define FIELDY_STEP 7
	#define FIELD_LBL_CHARS 6   // 4 chars + space
	int lblchars=1;
	if (showx) lblchars += 2;
	if (showy) lblchars += 2;
	float xcnt = ((float)tcols) / lblchars;
	float xrange = mr + mr*.9;
	float xstep = xrange / xcnt;
	float ystep = xrange / (((float)trows - statuslines));
	for (float y=-mr*.9; y<=mr*.9; y += ystep) {
		for (float x=-mr; x<=mr*.9; x += xstep) {
			fPair res;
			res = mousep->interp(x, y);
			cgotoxy(x/mr, y/mr);

			char *bgx, *bgy;
			bgx=bgy="\033[40m";
			if (res.x < -1.0)      bgx=rgb24bg_f( .8, .3, .0); 
			else if (res.x > 1.0)  bgx=rgb24bg_f( .8, .0, .4); 
			else {                 bgx=rgb24bg_f( abs(res.x)*.8 , 0, 0); }
			if (res.y < -1.0)      bgy=rgb24bg_f( .3, .8, .0); 
			else if (res.y > 1.0)  bgy=rgb24bg_f( .0, .8, .4); 
			else {                 bgy=rgb24bg_f(  0, abs(res.y)*.8, 0); }
			if (showx) printf("%s\033[37m%2d", bgx, int(fminf(res.x,.9)*10));
			if (showy) printf("%s\033[32m%2d", bgy, int(fminf(res.y,.9)*10));
			printf("\n");
		}
	}
	printf("\033[0m");
}

int main(int argc, char *argv[]) {
	int cur_pointi;
	bool showx=true, showy=false;
	Radterpolator mouse;
	fPair ptpairs[OT_CNT+1]; // theirs plus our Point and Center
	#ifdef SHOW_FS_FIELD
		float mr=310; // max range (to scale for plotting)
	#endif
	// Good test set
	ptpairs[T_LEFT].set(-300, 20);
	ptpairs[T_RIGHT].set(-200, 200);
	ptpairs[T_DOWN].set(10, -300);
	ptpairs[T_UP].set(100, 100);
	ptpairs[OT_CENTER].set(50, 50);
	ptpairs[OT_POINT].set(-290, 40);

	// Normal'ish person test set
	ptpairs[T_LEFT].set(-250, 55);
	ptpairs[T_RIGHT].set(250, -50);
	ptpairs[T_DOWN].set(30, -220);
	ptpairs[T_UP].set(-20, 200);
	ptpairs[OT_CENTER].set(40, -60);
	ptpairs[OT_POINT].set(-101, -59);

	update_radterp_system(&mouse, ptpairs);

	statuslines=3;
	term_init();    // We init even if SHOW_FS_FIELD is not defined
	                // so we can take user menu selections

	cur_pointi = OT_POINT;
	while (1) {
		fPair res;
		#ifdef SHOW_FS_FIELD
			cls();
			plot_field(&mouse, mr, showx, showy);
			draw_axii();
			gotostatus(2);
			printf("pid[%d] Center: %d,%d  Point: %d,%d",
					getpid(),
					int(PTCENTER.x), int(PTCENTER.y),
					int(PTPOINT.x), int(PTPOINT.y)
					);
			gotostatus(1);
		#endif
		printf("Currently moving: \033[33;1m%s\033[0m\n", pt_humanlabels[cur_pointi]);
		#ifdef SHOW_FS_FIELD
			gotostatus(0);
		#endif
		printf("(hjkl) move. (q)uit. Select point (%c%c%c%c %c %c). Toggle (x,y)",
			pt_keys[T_LEFT], pt_keys[T_RIGHT], pt_keys[T_UP], pt_keys[T_DOWN],
			pt_keys[OT_POINT], pt_keys[OT_CENTER]);
		#ifdef SHOW_FS_FIELD
			for (int i=0; i<POINTS; i++) {
				cgotoxy(ptpairs[i].x/mr, ptpairs[i].y/mr);
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
		res = mouse.interp(ptpairs[OT_POINT].x, ptpairs[OT_POINT].y);
		////////////////////////////

		#ifdef SHOW_FS_FIELD
			CGOTOPOINT(PTPOINT);
			printf("\033[31;44;1m%.1f\033[1;30m,\033[32;44;1m%.1f\033[0m", res.x, res.y);
		#endif
		char c;
		if (read(0, &c, 1)) {
			if (c=='q') break;
			if (c=='h') ptpairs[cur_pointi].x -= MOVEINC;
			else if (c=='l') ptpairs[cur_pointi].x += MOVEINC;
			else if (c=='j') ptpairs[cur_pointi].y -= MOVEINC;
			else if (c=='k') ptpairs[cur_pointi].y += MOVEINC;
			else if (c=='x') showx = !showx;
			else if (c=='y') showy = !showy;
			//else if (c=='s') save_pairs(ptpairs); // maybe later
			else {
				for (int i=0; i<OT_CNT; i++) {
					if (c == pt_keys[i]) {
						cur_pointi = i;
					}
				}
			}
			update_radterp_system(&mouse, ptpairs);
		}
		#ifndef SHOW_FS_FIELD
			printf("%c\n");
		#endif
	}
	return 0;
}

/* Video title and description:
Successful mouse interpolation of arbitrary user sensor positions
To assist users with disabilities (starting with one), I have to convert an arbitrary sensor space (gyroscope x and y "tilt" values) to the user's desired mouse cursor movements.  Due to such persons' limitations in movement, they may need to set a point representing, for example, "move up", anywhere in the tilting space of the mouse.  Then I must interpolate the x and y coordinates for final output mouse moves (max range of [-1,1]) in the x and y direction.  I will show the method of interpolation later.
*/

// vim: ts=4 sw=4
