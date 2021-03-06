/**
 * @file	Fbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.03.2009
 * @brief 	Fbo class declaration.
 */

#ifndef FBO_H_
#define FBO_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "declarations.h"

namespace ooctools {

/**
 * @class Fbo
 * @brief This class represents a Framebuffer-Object.
 * This is an element of the FboFactory. Right now it can be instantiated as stand-alone, which is not
 * preferred.
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
	void setDimensions(int width, int height);
	void drawColorFSQuad();
	void drawDepthFSQuad();
	void unbind();
	void clear();
	void clearDepth();
	void clearColor();
	GLuint getColorTexId(){return mPriColorTexture;};
	GLuint getDepthTexId(){return mPriDepthTexture;};
	inline bool isBound() const {return fboIsBound;};
	void setDepthTex(GLuint texId);
	GLuint createDepthTex();

	int mPriWidth;
	int mPriHeight;

	static void checkFbo(Fbo* _fbo);

private:
	GLuint mPriFBO;
	GLuint mPriDepthBuffer;
	GLuint mPriColorTexture;
	GLuint mPriDepthTexture;
	GLuint mPriColorBuffer;
	bool fboIsBound;

};

} // end of namespace OOCTools

#endif /* FBO_H_ */
