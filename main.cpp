#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h> // sleep
#include "termstuff.h"
#include "radterpolate.h"

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

void update_radterp_system(Radterpolator mouse, fPair *ptpairs) {
	mouse.set_left(   ptpairs[T_LEFT]   );
	mouse.set_right(  ptpairs[T_RIGHT]  );
	mouse.set_down(   ptpairs[T_DOWN]   );
	mouse.set_up(     ptpairs[T_UP]     );
	mouse.set_center( ptpairs[OT_CENTER] );
	mouse.prep();
}

int main(int argc, char *argv[]) {
	int cur_pointi;
	Radterpolator mouse;
	fPair ptpairs[OT_CNT+1]; // theirs plus our Point and Center
	float mr=310; // max range (to scale for plotting)
	ptpairs[T_LEFT].set(-300, 20);
	ptpairs[T_RIGHT].set(-200, 200);
	ptpairs[T_DOWN].set(10, -300);
	ptpairs[T_UP].set(100, 100);
	ptpairs[OT_CENTER].set(50, 50);
	ptpairs[OT_POINT].set(-290, 40);

	update_radterp_system(mouse, ptpairs);
	statuslines=3;
	term_init();

	cur_pointi = OT_POINT;
	while (1) {
		fPair res;
		cls();
		draw_axii();
		gotostatus(2);
		printf("Center: %d,%d", int(PTCENTER.x), int(PTCENTER.y));
		gotostatus(1);
		printf("(hjkl) move. (q)uit. Select point (%c%c%c%c %c %c)",
			pt_keys[T_LEFT], pt_keys[T_RIGHT], pt_keys[T_UP], pt_keys[T_DOWN],
			pt_keys[OT_POINT], pt_keys[OT_CENTER]);
		gotostatus(0);
		printf("Currently moving: \033[33;1m%s\033[0m", pt_humanlabels[cur_pointi]);
		for (int i=0; i<POINTS; i++) {
			cgotoxy(ptpairs[i].x/mr, ptpairs[i].y/mr);
			printf("%s", pt_syms[i]);
		}
		CGOTOPOINT(PTLEFT); printf("\033[31m%s\033[0m", PSYMLEFT);
		CGOTOPOINT(PTRIGHT); printf("\033[31m%s\033[0m", PSYMRIGHT);
		CGOTOPOINT(PTUP); printf("\033[32m%s\033[0m", PSYMUP);
		CGOTOPOINT(PTDOWN); printf("\033[32m%s\033[0m", PSYMDOWN);
		CGOTOPOINT(PTCENTER); printf("\033[36;1m%s\033[0m", PSYMCENTER);

		////////////////////////////
		// \/  Interpolate here
		res = mouse.interp(ptpairs[OT_POINT].x, ptpairs[OT_POINT].y);
		////////////////////////////
		CGOTOPOINT(PTPOINT);
		printf("\033[31;1m%.1f\033[1;30m,\033[32;1m%.1f\033[0m", res.x, res.y);
		char c;
		if (read(0, &c, 1)) {
			if (c=='q') break;
			if (c=='h') ptpairs[cur_pointi].x -= MOVEINC;
			else if (c=='l') ptpairs[cur_pointi].x += MOVEINC;
			else if (c=='j') ptpairs[cur_pointi].y -= MOVEINC;
			else if (c=='k') ptpairs[cur_pointi].y += MOVEINC;
			else {
				for (int i=0; i<OT_CNT; i++) {
					if (c == pt_keys[i]) {
						cur_pointi = i;
					}
				}
			}
			update_radterp_system(mouse, ptpairs);
		}
	}
	return 0;
}

/* Video title and description:
Successful mouse interpolation of arbitrary user sensor positions
To assist users with disabilities (starting with one), I have to convert an arbitrary sensor space (gyroscope x and y "tilt" values) to the user's desired mouse cursor movements.  Due to such persons' limitations in movement, they may need to set a point representing, for example, "move up", anywhere in the tilting space of the mouse.  Then I must interpolate the x and y coordinates for final output mouse moves (max range of [-1,1]) in the x and y direction.  I will show the method of interpolation later.
*/

// vim: ts=4 sw=4
