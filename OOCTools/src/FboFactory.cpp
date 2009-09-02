/**
 * @file	FboFactory.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.03.2009
 * @brief   Class definition of FboFactory
 */

#include "FboFactory.h"
#include "Fbo.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <iostream>

using namespace std;
namespace ooctools {

FboFactory* FboFactory::mPriInstance = 0;

FboFactory*
FboFactory::getSingleton()
{
	if (mPriInstance == 0){
		mPriInstance = new FboFactory();
	}
	return mPriInstance;
}

string
FboFactory::statusToString(GLenum status)
{
	if (status==GL_FRAMEBUFFER_COMPLETE_EXT) return string("complete");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT) return string("incomplete attachment");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT) return string("incomplete missing attachment");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_EXT) return string("incomplete layer count");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT) return string("incomplete layer targets");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT) return string("incomplete dimensions");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT) return string("incomplete formats");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT) return string("incomplete drawbuffer");
	else if (status==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT) return string("incomplete readbuffer");
	else if (status==GL_FRAMEBUFFER_UNSUPPORTED_EXT) return string("unsupported");
	else return string("unknown status");
}

FboFactory::FboFactory() {
	// TODO Auto-generated constructor stub - init stuff goes here
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &mPriMaxTexSize);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &mPriMaxColorAttach);
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE_EXT, &mPriMaxRenderbufferSize);
}

FboFactory::~FboFactory() {
	// TODO Auto-generated destructor stub
}

Fbo*
FboFactory::createFbo(int _width, int _height)
{
	//TODO
	Fbo *fbo = new Fbo(_width, _height);
	mPriFboList.insert(make_pair(fbo->getId(), fbo));
	return fbo;
}

Fbo*
FboFactory::createCompleteFbo(int _width, int _height)
{
	//TODO
	Fbo *fbo = new Fbo(_width, _height);
	fbo->createAndAddDepthBuf();
//	fbo->createAndAddColorBuf();
	fbo->createAndAddColorTex();
	//fbo->createAndAddDepthTex();
	mPriFboList.insert(make_pair(fbo->getId(), fbo));

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	cout << FboFactory::statusToString(status) << endl;

	return fbo;
}

void
FboFactory::checkFbo(GLuint id)
{
	if (id != mPriActiveFbo){
		mPriFboList[id]->bind();
	}
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	cout << FboFactory::statusToString(status) << endl;
}

void
FboFactory::checkFbo(Fbo* _fbo)
{
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	cout << FboFactory::statusToString(status) << endl;
	if (_fbo->getId() != mPriActiveFbo){
		mPriFboList[_fbo->getId()]->bind();
	}
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	cout << FboFactory::statusToString(status) << endl;
}

void
FboFactory::drawColorToFb(const GLubyte *pixels, int wPos, int hPos, int width, int height)
{

	// alternate method
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(0, width, 0, height, -1, 1);
//	glRasterPos2i(wPos,hPos);
//	glDrawPixels(width,height,GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)pixels);
//	glMatrixMode(GL_MODELVIEW);

	glWindowPos2i(wPos,hPos);
	glDrawPixels(width, height,GL_BGRA, GL_UNSIGNED_BYTE, (void*) pixels);
}

void
FboFactory::drawDepthToFb(const GLfloat* depth, int wPos, int hPos, int width, int height)
{
	glWindowPos2i(wPos, hPos);
	glDrawPixels(width, height,GL_DEPTH_COMPONENT, GL_FLOAT, depth);
}

void
FboFactory::readColorFromFb(GLubyte *pixels, int wPos, int hPos, int width, int height)
{
	glReadPixels(wPos,hPos,width,height,GL_BGRA, GL_UNSIGNED_BYTE, pixels);
}

void
FboFactory::readDepthFromFb(GLfloat *depth, int wPos, int hPos, int width, int height)
{
	glReadPixels(wPos,wPos,width,height,GL_DEPTH_COMPONENT, GL_FLOAT, depth);
}

} // end of Namespace OOCTools
