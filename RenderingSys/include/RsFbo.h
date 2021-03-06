/**
 * @file	RsFBO.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.06.2010
 * @brief 	RsFBO class declaration.
 */

#ifndef RSFBO_H_
#define RSFBO_H_

#include <GL/glew.h>

/**
 * @class RsFbo
 * @brief Simple framebuffer object implementations with a depth-buffer and a color texture.
 */
class RsFbo {
public:
	RsFbo(int _w, int _h);
	virtual ~RsFbo();
	void createAndAddDepthBuf();
	void createAndAddColorTex();
	GLuint getId(){return mPriFBO;};
	void bind();
	void unbind();
	void clear();
	void clearDepth();
	void clearColor();
	GLuint getColorTexId(){return mPriColorTexture;};
	void setSize(int _w, int _h);
private:
	int mPriWidth;
	int mPriHeight;
	GLuint mPriFBO;
	GLuint mPriDepthBuffer;
	GLuint mPriColorTexture;
	bool fboIsBound;

};

#endif /* RSFBO_H_ */
