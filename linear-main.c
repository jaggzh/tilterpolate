#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class fPoint {
	public:
		float x=0, y=0;
		fPoint() { x=0; y=0; }
		fPoint(float sx, float sy) { x=sx; y=sy; }
		void set(float sx, float sy) { x=sx; y=sy; }
		void set(fPoint p) { x=p.x; y=p.y; }
};

float sign(fPoint p1, fPoint p2, fPoint p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool PointInTriangle(fPoint pt, fPoint v1, fPoint v2, fPoint v3) {
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}


/*
-3	-1
1	0
2	0
5	1
6	0
*/
template <typename T>
class Point {
	public:
		T x=0, y=0;
		Point() { x=0; y=0; }
		Point(T sx, T sy) { x=sx; y=sy; }
		void set(T sx, T sy) { x=sx; y=sy; }
		void set(Point p) { x=p.x; y=p.y; }
};

enum { T_LEFT, T_RIGHT, T_TOP, T_BOTTOM, T_CENTER };
#define POINTS 5

template <typename T>
class LinearTilterpolator {
public:
	Point<T> left = Point<T>(0,0);
	Point<T> right = Point<T>(0,0);
	Point<T> up = Point<T>(0,0);
	Point<T> down = Point<T>(0,0);
	Point<T> center = Point<T>(0,0);
	Point<T> *sortx = new Point<T>[POINTS];
	Point<T> *sorty = new Point<T>[POINTS];
	static bool pt_lesserx(Point<T> p1, Point<T> p2) {
		return p1.x < p2.x;
	}
	static bool pt_lessery(Point<T> p1, Point<T> p2) {
		return p1.y < p2.y;
	}

	Tilterpolator(void) {
		for (int i=0; i<POINTS; i++) {
			sortx[i] = Point<T>(0,0);
			sorty[i] = Point<T>(0,0);
		}
	}
	void print(void) {
		printf("X values...\n");
		for (int i=0; i<POINTS; i++) {
			printf(" [%d] %d, %d\n", i, sortx[i].x, sortx[i].y);
		}
		printf("Y values...\n");
		for (int i=0; i<POINTS; i++) {
			printf(" [%d] %d, %d\n", i, sorty[i].x, sorty[i].y);
		}
	}
	void prep(void) {
		sortx[T_LEFT].set(left);
		sortx[T_LEFT] = sorty[T_LEFT] = left;
		sortx[T_RIGHT] = sorty[T_RIGHT] = right;
		sortx[T_TOP] = sorty[T_TOP] = up;
		sortx[T_BOTTOM] = sorty[T_BOTTOM] = down;
		sortx[T_CENTER] = sorty[T_CENTER] = center;
		std::sort(sortx, sortx+POINTS, pt_lesserx);
		std::sort(sorty, sorty+POINTS, pt_lessery);
	}
};

int main(int argc, char *argv[]) {
	Tilterpolator<int16_t> mouse;
	mouse.left.set(-3,-1);
	mouse.right.set(1,0);
	mouse.up.set(-2,3);
	mouse.center.set(5,1);
	mouse.down.set(6,0);
	mouse.prep();
	mouse.print();
	fPoint a,b,c,p;
	a = fPoint(mouse.left.x, mouse.left.y);
	b = fPoint(mouse.up.x, mouse.up.y);
	c = fPoint(mouse.center.x, mouse.center.y);
	p = fPoint(2,0);
	printf("In tri: %d\n", PointInTriangle(p, a, b, c));
	return 0;
}
