#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h> // sleep
#include "termstuff.h"
#include "radterpolate.h"

int main(int argc, char *argv[]) {
	Radterpolator mouse;
	float px=-290, py=40;
	float mr=300; // max range (to scale for plotting)
	float lx=-300, ly=20;
	float rx=-300, ry=100;
	float dx=10, dy=-300;
	float ux=100, uy=100;
	float cx=0, cy=50;

	mouse.set_left(lx,ly);
	mouse.set_right(rx,ry);
	mouse.set_down(dx,dy);
	mouse.set_up(ux,uy);
	mouse.set_center(cx,cy);
	mouse.prep();

	statuslines=2;
	init();
	while (1) {
		fPair res;
		cls();
		draw_axii();
		gotostatus(1);
		printf("HJKL to move. Q to quit (all lowercase).");
		cpat('L', lx/mr, ly/mr);
		cpat('R', rx/mr, ry/mr);
		cpat('D', dx/mr, dy/mr);
		cpat('U', ux/mr, uy/mr);
		cpat('C', cx/mr, cy/mr);
		////////////////////////////
		res = mouse.interp(px, py);    // <-------- Interpolate here
		////////////////////////////
		cgotoxy(px/mr, py/mr);
		printf("\033[31;1m%.1f\033[1;30m,\033[32;1m%.1f\033[0m", res.x, res.y);
		char c;
		if (read(0, &c, 1)) {
			if (c=='q') break;
			if (c=='h') px -= 5;
			else if (c=='l') px += 5;
			else if (c=='j') py -= 5;
			else if (c=='k') py += 5;
		}
	}
	return 0;
}

/* Video title and description:
Successful mouse interpolation of arbitrary user sensor positions
To assist users with disabilities (starting with one), I have to convert an arbitrary sensor space (gyroscope x and y "tilt" values) to the user's desired mouse cursor movements.  Due to such persons' limitations in movement, they may need to set a point representing, for example, "move up", anywhere in the tilting space of the mouse.  Then I must interpolate the x and y coordinates for final output mouse moves (max range of [-1,1]) in the x and y direction.  I will show the method of interpolation later.
*/

// vim: ts=4 sw=4
