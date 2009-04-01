/*
 * GlToolkit.cpp
 *
 *  Created on: 23.03.2009
 *      Author: ava
 */

#include "GlToolkit.h"

#include <iostream>

#include "BoundingBox.h"
#include "FboFactory.h"

namespace ooctools {

//TODO move to data-node class
float performOcclusionQuery(float *modelViewMatrix, BoundingBox *obj, GLfloat *depthBuffer,
		int wPos, int hPos, int width, int height)
{
	// prior to this ensure that no color-thingys(shader, light, etc) are enabled
	// also the frustum has to be adjusted to fit wPos/hPos and width/height

	// following global defines are to be removed from final implementation
	// they are not wanted in the display-loop
	GLuint oQuery;
	GLint singleResult;
	GLint sceneResult;
	glGenQueries(1, &oQuery);
	// end of global inits -----------------------------------
	glPushMatrix();
		//load modelViewMatrix to set the scene to same position as the master/render node
		glLoadMatrixf(modelViewMatrix);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBeginQuery(GL_SAMPLES_PASSED, oQuery);
		obj->drawSolid();
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(oQuery, GL_QUERY_RESULT, &singleResult);
		glClear(GL_DEPTH_BUFFER_BIT);
		// the actual test:
		// writing the depthbuffer
		glPushAttrib(GL_ALL_ATTRIB_BITS);
			glDepthFunc(GL_ALWAYS);
			glShadeModel(GL_FLAT);
			FboFactory::getInstance()->drawDepthToFb(depthBuffer, wPos, hPos,width, height);
		glPopAttrib();
		glClear(GL_COLOR_BUFFER_BIT);
		glBeginQuery(GL_SAMPLES_PASSED, oQuery);
		obj->drawSolid();
		glEndQuery(GL_SAMPLES_PASSED);
		glGetQueryObjectiv(oQuery, GL_QUERY_RESULT, &sceneResult);
	glPopMatrix();

	return singleResult/sceneResult;
}

} // end of namespace ooctools
