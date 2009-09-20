/**
 * @file	Fbo.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.03.2009
 * @brief   Class definition of Fbo
 */

#include "Fbo.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cstring>

using namespace std;
namespace ooctools {

Fbo::Fbo(int _width, int _height) : fboIsBound(false)
{
	// TODO Auto-generated constructor stub
	mPriWidth = _width;
	mPriHeight = _height;
	glGenFramebuffersEXT(1, &mPriFBO);
}

Fbo::~Fbo()
{
	// TODO Auto-generated destructor stub

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);

	glDeleteRenderbuffersEXT(1, &mPriDepthBuffer);
	glDeleteRenderbuffersEXT(1, &mPriColorBuffer);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &mPriColorTexture);
	glDeleteTextures(1, &mPriDepthTexture);

	glDeleteFramebuffersEXT(1, &mPriFBO);

}

void
Fbo::bind()
{
	if (!fboIsBound){
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mPriFBO);
		fboIsBound = true;
	}
}

void
Fbo::unbind()
{
	if (fboIsBound){
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		fboIsBound = false;
	}
}

void
Fbo::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
Fbo::clearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void
Fbo::clearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void
Fbo::addDepthBuf(GLuint id)
{
	//TODO evtl. remove and delete a buffer that was created before
	mPriDepthBuffer = id;
	// set current renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mPriDepthBuffer);
	// associate storage space with bound renderbuffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mPriWidth, mPriHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	// attaching renderbuffer/depthbuffer to the FBO
	bind();
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mPriDepthBuffer);
	unbind();
}

void
Fbo::createAndAddDepthBuf()
{
	//TODO evtl. remove and delete a buffer that was created before
	glGenRenderbuffersEXT(1, &mPriDepthBuffer);
	// set current renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mPriDepthBuffer);
	// associate storage space with bound renderbuffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mPriWidth, mPriHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// attaching renderbuffer/depthbuffer to the FBO
	bind();
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mPriDepthBuffer);
	unbind();
}

void
Fbo::setDimensions(int width, int height)
{
	mPriWidth = width;
	mPriHeight = height;
	if(mPriDepthBuffer!=0){
		// set current renderbuffer
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mPriDepthBuffer);
		// associate storage space with bound renderbuffer
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, mPriWidth, mPriHeight);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
		bind();
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mPriDepthBuffer);
		unbind();
	}

	if (mPriColorTexture!=0){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPriColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, mPriWidth, mPriHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		bind();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mPriColorTexture, 0);
		unbind();

	}
}

// this did not work
void
Fbo::createAndAddColorBuf()
{
	//TODO evtl. remove and delete a buffer that was created before
	glGenRenderbuffersEXT(1, &mPriColorBuffer);
	// set current colorbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, mPriColorBuffer);
	// associate storage space with bound renderbuffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_BGRA, mPriWidth, mPriHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// attaching renderbuffer/depthbuffer to the FBO
	bind();
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, mPriColorBuffer);
	unbind();
}

void
Fbo::createAndAddColorTex()
{
	glGenTextures(1, &mPriColorTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriColorTexture);
	// neccessary - default will modulate with current set glColor
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	// reserving space for the colorbuffer
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, mPriWidth, mPriHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, mPriWidth, mPriHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attaching texture to FBO
	bind();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, mPriColorTexture, 0);
//	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mPriTexture, 0);
	unbind();
}

void
Fbo::createAndAddDepthTex()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mPriFBO);
	glGenTextures(1, &mPriDepthTexture);
	glBindTexture(GL_TEXTURE_2D, mPriDepthTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	// somehow this little sucker causes the texture still to be valid, eg. it renders
	// all depth correctly, but it can't be displayed anymore.
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mPriWidth, mPriHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, mPriDepthTexture, 0);
	unbind();
}

void Fbo::drawColorFSQuad()
{
	bool texOn = true;
	if (!glIsEnabled(GL_TEXTURE_2D)) {
		glEnable(GL_TEXTURE_2D);
		texOn = false;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriColorTexture);

	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode (GL_PROJECTION);
		glPushMatrix ();
			glLoadIdentity ();
			glBegin (GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3i (-1, -1, -1);
				glTexCoord2f(1.0f, 0.0f); glVertex3i (1, -1, -1);
				glTexCoord2f(1.0f, 1.0f); glVertex3i (1, 1, -1);
				glTexCoord2f(0.0f, 1.0f); glVertex3i (-1, 1, -1);
			glEnd ();
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();

	if (!texOn){
		glDisable(GL_TEXTURE_2D);
	}
}

void Fbo::drawDepthFSQuad(){
	bool texOn = true;
	if (!glIsEnabled(GL_TEXTURE_2D)) {
		glEnable(GL_TEXTURE_2D);
		texOn = false;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPriDepthTexture);

	glMatrixMode (GL_MODELVIEW);
	glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode (GL_PROJECTION);
		glPushMatrix ();
			glLoadIdentity ();
			glBegin (GL_QUADS);
				glTexCoord2f(0.0f, 0.0f); glVertex3i (-1, -1, -1);
				glTexCoord2f(1.0f, 0.0f); glVertex3i (1, -1, -1);
				glTexCoord2f(1.0f, 1.0f); glVertex3i (1, 1, -1);
				glTexCoord2f(0.0f, 1.0f); glVertex3i (-1, 1, -1);
			glEnd ();
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();

	if (!texOn){
		glDisable(GL_TEXTURE_2D);
	}

}

} // end of namespace OOCTools
