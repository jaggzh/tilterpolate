#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h> // sleep
#include "termstuff.h"

//#define TILT_VERBOSE 2
#if defined(TILT_VERBOSE) && TILT_VERBOSE > 0
	#define verb_printf(...) printf(__VA_ARGS__)
#else
	#define verb_printf(...)
#endif

using namespace std;

enum { T_LEFT, T_RIGHT, T_UP, T_DOWN };
const char *cnames[] = { "T_LEFT", "T_RIGHT", "T_UP", "T_DOWN" };
#define POINTS 4

#define R2D(r) ((r)*180/M_PI)
#define D2R(r) ((r)*M_PI/180)

class fPair {
	public:
		float x,y;
		fPair() { x=y=0; }
		fPair(float sx, float sy) { x=sx, y=sy; }
};

class mPoint { // mPoint with mapping info
	public:
		float x=0, y=0;         // x,y coord input
		float map_to_x=0, map_to_y=0;  // mapping (destination) values
		float anglec=0;           // Angle to center point
		float scalerx=0, scalery=0;
		mPoint() {
			x=y=map_to_x=map_to_y=anglec=0;
			scalerx=scalery=0;
		}
		mPoint(const mPoint &p) {
			x=p.x; y=p.y;
			map_to_x=p.map_to_x; map_to_y=p.map_to_y;
			anglec=p.anglec;
			scalerx=p.scalerx; scalery=p.scalery;
		}
		mPoint(float posx, float posy)
			{ x=posx; y=posy; }
		mPoint(float posx, float posy, float destx, float desty) {
			x=posx; y=posy;
			map_to_x=destx; map_to_y=desty;
		}
		void set(float posx, float posy, float destx, float desty)
			{ x=posx; y=posy; map_to_x=destx; map_to_y=desty; }
		void set(mPoint p) { x=p.x; y=p.y; map_to_x=p.map_to_x; map_to_y=p.map_to_y; anglec=p.anglec; scalerx=p.scalerx; scalery=p.scalery; }
		float set_angle_to(mPoint p) { return anglec = angle_to(p); }
		float angle_to(mPoint p) { return atan2(y-p.y, x-p.x); }
		void calc_scalers() {
			float squirt = sqrt(x*x+y*y);
			scalerx = map_to_x/squirt;
			scalery = map_to_y/squirt;
		}
		float get_radius() { return sqrt(x*x+y*y); }
		void print(const char *label) {
			printf(" Point(%s) (xy: %.3f, %.3f) Angle: %.3f (mx,y: %.2f, %.2f)\n",
				"?", x, y,
				R2D(atan2(y, x)),
				map_to_x, map_to_y
				);
		}
};

class Radterpolator {
public:
	mPoint *points = new mPoint[POINTS];
	mPoint *sorted = new mPoint[POINTS];
	mPoint center;
	Radterpolator(void) {
		for (int i=0; i<POINTS; i++) {
			points[i].set(0,0,0,0);
		}
	}
	void set_left(float x, float y)  { points[T_LEFT].set(x, y, -1, 0); }
	void set_right(float x, float y) { points[T_RIGHT].set(x, y, 1, 0); }
	void set_up(float x, float y)    { points[T_UP].set(x, y, 0, 1); }
	void set_down(float x, float y)  { points[T_DOWN].set(x, y, 0, -1); }
	void set_center(float x, float y)  { center.set(x, y, 0, 0); }
	void print(void) {
		printf("values...\n");
		for (int i=0; i<POINTS; i++) {
			printf(" [%d] %f, %f\n", i, sorted[i].x, sorted[i].y);
		}
	}
	static bool sort_angle(mPoint p1, mPoint p2) {
		return atan2(p2.y, p2.x) < atan2(p1.y, p1.x);
	}
	double get_rangle_to_center(float x, float y) {
		return atan2(y - center.y,   x - center.x);
	}
	double get_rangle_to_center(mPoint p) {
		return atan2(p.y - center.y,   p.x - center.x);
	}
	void calc_angles(void) {
		for (int i=0; i<POINTS; i++) {
			points[i].anglec = get_rangle_to_center(points[i]);
		}
	}
	void prep(void) {
		#if defined(TILT_VERBOSE) && TILT_VERBOSE > 0
			printf("Center: %f, %f\n", center.x, center.y);
		#endif
		for (int i=0; i<POINTS; i++) {
			// Set sorted array to points offset around CENTER
			#if defined(TILT_VERBOSE) && TILT_VERBOSE > 0
				printf("mPoint orig [%d](%s) (xy: %.3f, %.3f)",
					i, cnames[i], points[i].x, points[i].y);
			#endif
			sorted[i].set(
					points[i].x - center.x,
					points[i].y - center.y,
					points[i].map_to_x,
					points[i].map_to_y
					);
			sorted[i].set_angle_to(mPoint(0,0)); // we're offset now so we can use 0,0
			sorted[i].calc_scalers();
			#if defined(TILT_VERBOSE) && TILT_VERBOSE > 0
				printf("  offset -> [%d](%s) (xy: %.3f, %.3f) Angle: %.3f (mx,y: %.2f, %.2f)\n",
					i, cnames[i], sorted[i].x, sorted[i].y,
					R2D(sorted[i].anglec),
					sorted[i].map_to_x, sorted[i].map_to_y
					);
			#endif
		}
		#if defined(TILT_VERBOSE) && TILT_VERBOSE > 0
			printf("Unsorted:\n");
			print_points(sorted);
		#endif
		std::sort(sorted, sorted+POINTS, sort_angle);
		#if defined(TILT_VERBOSE) && TILT_VERBOSE > 0
			printf("Sorted:\n");
			print_points(sorted);
		#endif
	}
	void print_points(mPoint *list) {
		for (int i=0; i<POINTS; i++) list[i].print("?");
	}

/*
1. Get angles of Lines (L_1, L_2, L_p), ie. a_1, a_2, a_p
2. Find each main line's scaler (multiplier).
    Ex. #5 above, the vector is maybe at (-2,1.8), but with an interpolation value
    of 5.  if it were -//2/2,//2/2 its scaler would be 5. Larger mag == lower scaler
    ... So, scaler = val / sqrt(x^2+y^2)
3. Get radius of P: r_p = sqrt(px^2 + py^2)
3. Find contribution (c) of each line on P, by angle: ex. c_1 = (a_p-a_1)/a_total
4. Finally, handle the interpolation:
  4a.  Value = (r_p*c2*m2)-(r_p*c1*m1) = r_p(c2*m2-c1*m1)
*/

	fPair interp(float x, float y) { return interp(mPoint(x,y)); }
	fPair interp(mPoint p) { // !! prep() must be run first or we're unhappy
		fPair ret;
		verb_printf("Interpolating:\n");
		p.set_angle_to(center);
		verb_printf(" Center x,  y: [%.2f, %.2f]\n", center.x, center.y);
		verb_printf(" Point  x,  y: [%.2f, %.2f]\n", p.x, p.y);
		//verb_printf(" Point dx, dy: [%.2f, %.2f]\n", p.x-center.x, p.y-center.y);
		verb_printf(" Point angle:%.3f\n", R2D(p.anglec));
		for (int i=0; i<POINTS; i++) {
			mPoint p1, p2;
			p1 = sorted[i];

				// At the last point we're going to need special handling
			if (i<POINTS-1) {
				verb_printf("  P1[%d], P2[%d]", i, i+1);
				p2 = sorted[i+1];
			} else {  // <------------------- last point
				verb_printf("  \033[33;1m*[FLIP] P1[%d], P2[%d]\033[0m\n", i, 0);
				p2 = sorted[0];
				p2.anglec -= 2*M_PI;
			}
			verb_printf(" p1a:%.3f <= ", R2D(p1.anglec));
			verb_printf("%.3f < ", R2D(p.anglec));
			verb_printf("p2a:%.3f", R2D(p2.anglec));
			verb_printf("\n");
			if (p.anglec<=p1.anglec && p.anglec>p2.anglec) {
				verb_printf("  !! Point is between me [%d and %d]\n", i, i+1);
				// Point p is now destroyed!
				p.x -= center.x;
				p.y -= center.y;
				// Point p is now destroyed!
				float prad = p.get_radius();
				verb_printf("  PRAD: %.2f\n", prad);
				// don't forget: Angles decrease from p1 to p2
				float angtot = p1.anglec - p2.anglec;
				verb_printf("  Angle difference p1-p: %.3f\n",(p1.anglec - p.anglec));
				verb_printf("  Angle difference p-p2: %.3f\n",(p.anglec - p2.anglec));
				float c1 = 1-(p1.anglec - p.anglec)/angtot;
				float c2 = 1-((p.anglec - p2.anglec)/angtot);
				verb_printf("  \033[36;1mContributions 1&2: %.3f, %.3f\033[0m\n", c1, c2);
				verb_printf("p1,p2 scax: %.3f %.3f\n", p1.scalerx, p2.scalerx);
				verb_printf("p1,p2 scay: %.3f %.3f\n", p1.scalery, p2.scalery);
				float valx = prad * (c2*p2.scalerx + c1*p1.scalerx);
				float valy = prad * (c2*p2.scalery + c1*p1.scalery);
				verb_printf("   \033[33;1mFINAL mx,y: %.3f, %.3f\033[0m\n", valx, valy);
				ret.x = valx;
				ret.y = valy;
				return ret;
			}
		}
		verb_printf("\033[31;1mFailed to find where the point belongeth.\033[0m\n");
		return ret;
	}
};

int main(int argc, char *argv[]) {
	Radterpolator mouse;
	float px=-290, py=40;
	float mr=300; // max range (to scale for plotting)
	float lx=-300, ly=20;
	float rx=-300, ry=100;
	float dx=10, dy=300;
	float ux=100, uy=100;
	float cx=0, cy=0;

	mouse.set_left(lx,ly);
	mouse.set_right(rx,ry);
	mouse.set_down(dx,dy);
	mouse.set_up(ux,uy);
	mouse.set_center(cx,cy);
	mouse.prep();

	init();
	while (1) {
		fPair res;
		cls();
		draw_axii();
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

// vim: ts=2 sw=2
