/**
 * @file	RsFBO.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.06.2010
 * @brief 	RsFBO class declaration.
 */

#include "RsFBO.h"

#include <iostream>

RsFBO::RsFBO(int _w, int _h) {
	mPriWidth = _w;
	mPriHeight = _h;
	glGenFramebuffersEXT(1, &mPriFBO);
}

RsFBO::~RsFBO() {
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);


	glDeleteRenderbuffersEXT(1, &mPriDepthBuffer);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &mPriColorTexture);

	glDeleteFramebuffersEXT(1, &mPriFBO);
}

void RsFBO::createAndAddDepthBuf()
{
	//TODO evtl. remove and delete a buffer that was created before
	glGenRenderbuffersEXT(1, &this->mPriDepthBuffer);
	// set current renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->mPriDepthBuffer);
	// associate storage space with bound renderbuffer
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->mPriWidth, this->mPriHeight);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// attaching renderbuffer/depthbuffer to the FBO
	this->bind();
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->mPriDepthBuffer);
	this->unbind();
}

void RsFBO::createAndAddColorTex()
{
	glGenTextures(1, &this->mPriColorTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->mPriColorTexture);
	// neccessary - default will modulate with current set glColor
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// reserving space for the colorbuffer
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, this->mPriWidth, this->mPriHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attaching texture to FBO
	this->bind();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->mPriColorTexture, 0);
	this->unbind();
}

void RsFBO::bind()
{
	if (!fboIsBound){
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mPriFBO);
		fboIsBound = true;
	}
}

void RsFBO::unbind()
{
	if (fboIsBound){
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		fboIsBound = false;
	}
}

void RsFBO::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RsFBO::clearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);

}

void RsFBO::clearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);

}

void RsFBO::setSize(int _w, int _h)
{
	mPriWidth = _w;
	mPriHeight = _h;
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	if (mPriDepthBuffer != 0){	// "resizing" the depth attachment
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, 0);
		this->createAndAddDepthBuf();
	}
	if (mPriColorTexture != 0){	// "resizing" the color-attachment
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDeleteTextures(1, &mPriColorTexture);
		this->createAndAddColorTex();
	}
	//TODO
}
