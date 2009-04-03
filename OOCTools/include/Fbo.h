/**
 * @file	Fbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.03.2009
 *
 */

#ifndef FBO_H_
#define FBO_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "declarations.h"

namespace ooctools {

/**
 * @class C
 * @brief B
 * L
 */
class Fbo
{
	friend class FboFactory;
public:
	Fbo(int _width, int _height);
	virtual ~Fbo();
	GLuint getId(){return mPriFBO;};
	void bind();
	void addDepthBuf(GLuint id);
	void createAndAddDepthBuf();
	void createAndAddColorTex();
	void createAndAddDepthTex();
	void createAndAddColorBuf();
	void unbind();
	void clear();
	GLuint getColorTexId(){return mPriColorTexture;};
	GLuint getDepthTexId(){return mPriDepthTexture;};

private:
	GLuint mPriFBO;
	GLuint mPriDepthBuffer;
	GLuint mPriColorTexture;
	GLuint mPriDepthTexture;
	GLuint mPriColorBuffer;
	int mPriWidth;
	int mPriHeight;


};

} // end of namespace OOCTools

#endif /* FBO_H_ */
