#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "platform.h"
#include "trackball.h"

#define PI 3.141592653589793

/* Maps the mouse pointer on the screen plane to the trackball's sphere.
 *     - code graciously borrow from Ed Angel’s OpenGL Primer.
 * -------------------------------------------------------------------------- */
void mapToTrackball(int x, int y, int window_size, float v[3]) {
	float d, a;
	/* project x,y onto a hemi-sphere centered within width, height */
	v[0] = (2.0F*x - window_size) / window_size;
	v[1] = (window_size - 2.0F * y) / window_size;
	d = (float) sqrt(v[0] * v[0] + v[1] * v[1]);
	v[2] = (float) cos((PI/2.0F) * ((d < 1.0F) ? d : 1.0F));
	a = 1.0F / (float) sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	
	v[0] *= a;
	v[1] *= a;
	v[2] *= a;
}

/* Updates the passed trackball using information about the mouse position
 * and window size.
 *     - code graciously borrow from Ed Angel’s OpenGL Primer.
 * -------------------------------------------------------------------------- */
void updateTrackball(int x, int y, int window_size, float *last_pos, Trackball *trackball) {
	float curPos[3], dx, dy, dz;

	mapToTrackball(x, y, window_size, curPos);
	if(true) {
		dx = curPos[0] - last_pos[0];
		dy = curPos[1] - last_pos[1];
		dz = curPos[2] - last_pos[2];

		if (dx || dy || dz)	{
			trackball->angle = 90.0F * sqrt(dx*dx + dy*dy + dz*dz);
			trackball->trackball_rot = glm::rotate(trackball->trackball_rot, trackball->angle, trackball->axis[0], trackball->axis[1], trackball->axis[2]);

			trackball->axis[0] = last_pos[1]*curPos[2] - last_pos[2]*curPos[1];
			trackball->axis[1] = last_pos[2]*curPos[0] - last_pos[0]*curPos[2];
			trackball->axis[2] = last_pos[0]*curPos[1] - last_pos[1]*curPos[0];
			last_pos[0] = curPos[0];
			last_pos[1] = curPos[1];
			last_pos[2] = curPos[2];
		}
	}
}