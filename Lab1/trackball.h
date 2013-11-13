#ifndef TRACKBALL_H
#define TRACKBALL_H

/* --------------------------------------------------------------------------
 *  Defines functions to make using a trackball simple.
 * -------------------------------------------------------------------------- */

typedef struct {
	float angle;
	float axis[3];
	glm::mat4 trackball_rot;
} Trackball;

/* Maps the mouse pointer on the screen plane to the trackball's sphere.
 *     - code graciously borrow from Ed Angel’s OpenGL Primer.
 * -------------------------------------------------------------------------- */
void mapToTrackball(int x, int y, int window_size, float v[3]);

/* Updates the passed trackball using information about the mouse position
 * and window size.
 *     - code graciously borrow from Ed Angel’s OpenGL Primer.
 * -------------------------------------------------------------------------- */
void updateTrackball(int x, int y, int window_size, float *lastPos, Trackball *trackball);

#endif