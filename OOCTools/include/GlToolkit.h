/**
 * @file	GlToolkit.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.02.2009
 * @brief 	Namespace-wide OpenGL-related includes, definitions and declarations.
 */

#ifndef GLTOOLKIT_H_
#define GLTOOLKIT_H_

#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glu.h>

#include "declarations.h"
#include "V3f.h"
#include "V3ub.h"

namespace ooctools {

static V3f defaultColorF((float)0.78f, (float)0.57f, (float)0.11f);
static V3ub defaultColorB((unsigned char)199, (unsigned char)145, (unsigned char)28);


void inline getGlError(int ret)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		fprintf(stderr, "[%s line %d] GL Error: %s\n", __FILE__,
				__LINE__, gluErrorString(err));
		fflush(stderr);
	}
}

inline char* bufferOffset(int i){return (char *)0 + (i);}
float performOcclusionQuery(float *modelViewMatrix, BoundingBox *obj, GLfloat *depthBuffer,
		int wPos, int hPos, int width, int height);


} // end of namespace ooctools

#endif /* GLTOOLKIT_H_ */
