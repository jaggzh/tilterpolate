#ifndef RADTERPOLATE_H
#define RADTERPOLATE_H
#include <stdio.h>
#include <math.h>

/* #define RAD_VERBOSE 2 // lots of stuff to enjoy. Undefined or 0 is off */

enum { T_LEFT, T_RIGHT, T_UP, T_DOWN };

#define POINTS 4
#ifndef _IN_RADTERPOLATE_C
extern const char **cnames[POINTS+1]; // "T_LEFT", "T_RIGHT", "T_UP", "T_DOWN", "oof!"
#endif

#define R2D(r) ((r)*180/M_PI)
#define D2R(r) ((r)*M_PI/180)

#if defined(RAD_VERBOSE) && RAD_VERBOSE > 0
	#define verb_printf(...) printf(__VA_ARGS__)
#else
	#define verb_printf(...)
#endif

class fPair {
	public:
		float x,y;
		fPair() { x=y=0; }
		fPair(float sx, float sy) { x=sx, y=sy; }
		void set(float sx, float sy) { x=sx, y=sy; }
};

class mPoint { // mPoint with mapping info
	public:
		float x=0, y=0;         // x,y coord input
		float map_to_x=0, map_to_y=0;  // mapping (destination) values
		float anglec=0;           // Angle to center point
		float scalerx=0, scalery=0;
		const char *label="?";
		mPoint() {
			x=y=map_to_x=map_to_y=anglec=0;
			scalerx=scalery=0;
		}
		mPoint(const mPoint &p) {
			x=p.x; y=p.y;
			map_to_x=p.map_to_x; map_to_y=p.map_to_y;
			anglec=p.anglec;
			scalerx=p.scalerx; scalery=p.scalery;
			label=p.label;
		}
		mPoint(float posx, float posy)
			{ x=posx; y=posy; }
		mPoint(float posx, float posy, float destx, float desty) {
			x=posx; y=posy;
			map_to_x=destx; map_to_y=desty;
		}
		void set_label(const char *s) { label=s; }
		void set(float posx, float posy, float destx, float desty,
		         const char *slabel)
			{ x=posx; y=posy; map_to_x=destx; map_to_y=desty; label=slabel; }
		void set(float posx, float posy, float destx, float desty)
			{ x=posx; y=posy; map_to_x=destx; map_to_y=desty; }
		void set(mPoint p) {
			x=p.x; y=p.y; map_to_x=p.map_to_x; map_to_y=p.map_to_y;
			anglec=p.anglec; scalerx=p.scalerx; scalery=p.scalery;
		}
		float set_angle_to(mPoint p) { return anglec = angle_to(p); }
		float angle_to(mPoint p) { return atan2(y-p.y, x-p.x); }
		void calc_scalers() {
			float squirt = sqrt(x*x+y*y);
			scalerx = map_to_x/squirt;
			scalery = map_to_y/squirt;
		}
		float get_radius() { return sqrt(x*x+y*y); }
		float distance_to(mPoint p) {
			p.x -= x;  p.y -= y;
			return sqrt(x*x+y*y);
		}
		void print() {
			printf(" Point(%s) (xy: %.3f, %.3f) Angle: %.3f (mx,y: %.2f, %.2f)\n",
				label, x, y,
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
			points[i].set(0,0,0,0,"");
		}
	}

	void set_left(fPair p)
		{ points[T_LEFT].set(p.x, p.y, -1, 0, "LEFT"); }
	void set_right(fPair p)
		{ points[T_RIGHT].set(p.x, p.y, 1, 0, "RIGHT"); }
	void set_up(fPair p)
		{ points[T_UP].set(p.x, p.y, 0, 1, "UP"); }
	void set_down(fPair p)
		{ points[T_DOWN].set(p.x, p.y, 0, -1, "DOWN"); }
	void set_center(fPair p)
		{ center.set(p.x, p.y, 0, 0, "CENTER"); }

	void set_left(float x, float y)
		{ points[T_LEFT].set(x, y, -1, 0, "LEFT"); }
	void set_right(float x, float y)
		{ points[T_RIGHT].set(x, y, 1, 0, "RIGHT"); }
	void set_up(float x, float y)
		{ points[T_UP].set(x, y, 0, 1, "UP"); }
	void set_down(float x, float y)
		{ points[T_DOWN].set(x, y, 0, -1, "DOWN"); }
	void set_center(float x, float y)
		{ center.set(x, y, 0, 0, "CENTER"); }

	void print(void) {
		printf("values...\n");
		for (int i=0; i<POINTS; i++) {
			printf(" [%d (%s)] %f, %f\n",
				i, sorted[i].label, sorted[i].x, sorted[i].y);
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

	// Code for the following is in .cpp
	void prep(void);
	void print_points(mPoint *list);
	fPair interp(float x, float y) { return interp(mPoint(x,y)); }
	fPair interp(mPoint p); /* !! prep() must be run after setting points,
							   before calling interp(), or interp() might
							   be unhappy */
};

#endif
