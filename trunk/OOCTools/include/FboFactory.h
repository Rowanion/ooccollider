/**
 * @file	FboFactory.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.03.2009
 * @brief 	FboFactory class declaration.
 */

#ifndef FBOFACTORY_H_
#define FBOFACTORY_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <map>
#include <string>

#include "declarations.h"

namespace ooctools {


/**
 * @class FboFactory
 * @brief Manager-Class for everything around Framebuffer-Objects.
 * Here you can create, delete, setup, bind and unbind FBOs.
 */
class FboFactory
{
public:
	virtual ~FboFactory();
	static FboFactory* getSingleton();
	static std::string statusToString(GLenum status);
	Fbo *createFbo(int _width, int _height);
	Fbo *createCompleteFbo(int _width, int _height);
	void cleanup();
	const int getMaxTexSize(){return (const int) mPriMaxTexSize;}; // maximum supported texture size
	const int getMaxColorAttach(){return (const int) mPriMaxColorAttach;}; //maximum number of color buffers
	void checkFbo(GLuint id);
	void checkFbo(Fbo *_fbo);
	void drawColorToFb(GLubyte *pixels, int wPos, int hPos, int width, int height);
	void drawDepthToFb(GLfloat *depth, int wPos, int hPos, int width, int height);
	void readColorFromFb(GLubyte *pixels, int wPos, int hPos, int width, int height);
	void readDepthFromFb(GLfloat *depth, int wPos, int hPos, int width, int height);

private:
	static FboFactory *mPriInstance;
	std::map<GLuint, Fbo*> mPriFboList;
	int mPriMaxTexSize;
	int mPriMaxColorAttach;
	int mPriMaxRenderbufferSize;
	GLuint mPriActiveFbo;

	FboFactory();
};

} // end of Namespace OOCTools

#endif /* FBOFACTORY_H_ */
