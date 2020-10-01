#include <stdio.h>
#include <algorithm>
#include <sys/types.h> // getpid()  // used for attaching with debugger
#include <unistd.h>    // getpid()
#define _IN_RADTERPOLATE_C
#include "radterpolate.h"

const char *cnames[POINTS+1] = { "T_LEFT", "T_RIGHT", "T_UP", "T_DOWN", "oof!" };

void Radterpolator::prep(void) {
	#if defined(RAD_VERBOSE) && RAD_VERBOSE > 0
		printf("pid[%d] Center: %f, %f\n", getpid(), center.x, center.y);
	#endif
	for (int i=0; i<POINTS; i++) {
		mPoint origin(0,0);
		// Set sorted array to points offset around CENTER
		#if defined(RAD_VERBOSE) && RAD_VERBOSE > 0
			printf("mPoint orig [%d](%s) (xy: %.3f, %.3f)\n",
				i, cnames[i], points[i].x, points[i].y);
		#endif
		sorted[i].set(
				points[i].x - center.x,
				points[i].y - center.y,
				points[i].map_to_x,
				points[i].map_to_y,
				points[i].label
				);
		sorted[i].set_angle_to(origin); // we're offset now so we can use 0,0
		sorted[i].calc_scalers();
		#if defined(RAD_VERBOSE) && RAD_VERBOSE > 0
			printf("  offset -> [%d](%s) (xy: %.3f, %.3f) Angle: %.3f (mx,y: %.2f, %.2f)\n",
				i, cnames[i], sorted[i].x, sorted[i].y,
				R2D(sorted[i].anglec),
				sorted[i].map_to_x, sorted[i].map_to_y
				);
		#endif
	}
	#if defined(RAD_VERBOSE) && RAD_VERBOSE > 0
		printf("Unsorted:\n");
		print_points(sorted);
	#endif
	std::sort(sorted, sorted+POINTS, sort_angle);

	#if defined(RAD_VERBOSE) && RAD_VERBOSE > 0
		printf("Sorted:\n");
		print_points(sorted);
	#endif
}
void Radterpolator::print_points(mPoint *list) {
	for (int i=0; i<POINTS; i++) list[i].print();
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

fPair Radterpolator::interp(mPoint p) { /* !! prep() must be run after setting points,
						   before calling interp(), or interp() might
						   be unhappy */
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
			// It is the lowest angle, and is therefor a jump in pos/neg
			// back to point[0]
		if (i<POINTS-1) {
			verb_printf("  P1[%d], P2[%d]", i, i+1);
			p2 = sorted[i+1];
		} else {  // <---------- last point means a jump from lowest angle back to highest
			verb_printf("  \033[33;1m*[FLIP] P1[%d], P2[%d]\033[0m\n", i, 0);
			p2 = sorted[0];
			//p2.anglec -= 2*M_PI;
			p2.anglec -= 2*M_PI;
			if (p.anglec > sorted[0].anglec) p.anglec -= 2*M_PI;
		}
		verb_printf(" p1a:%.3f <= ", R2D(p1.anglec));
		verb_printf("%.3f < ", R2D(p.anglec));
		verb_printf("p2a:%.3f", R2D(p2.anglec));
		verb_printf("\n");
		if (p.anglec<=p1.anglec && p.anglec>p2.anglec) {
			verb_printf("  !! Point is between me [%d and %d]\n", i, i+1);
			float prad = p.distance_to(center);
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
			ret.x = prad * (c2*p2.scalerx + c1*p1.scalerx);
			ret.y = prad * (c2*p2.scalery + c1*p1.scalery);
			verb_printf("   \033[33;1mFINAL mx,y: %.3f, %.3f\033[0m\n", ret.x, ret.y);
			return ret;
		}
	}
	verb_printf("\033[31;1mFailed to find where the point belongeth.\033[0m\n");
	return ret;
}

