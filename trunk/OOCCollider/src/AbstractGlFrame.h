/**
 * @file	AbstractGlFrame.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	AbstractGlFrame class declaration.
 */

#ifndef ABSTRACTGLFRAME_H_
#define ABSTRACTGLFRAME_H_

#include "declarations.h"

/**
 * @class AbstractGlFrame
 * @brief Abstract class with basic OpenGL-Methods such as init, display, reshape.
 */
class AbstractGlFrame {
public:
	virtual ~AbstractGlFrame(){};
	virtual void init() = 0;
	virtual void display() = 0;
	virtual void reshape(int width, int height) = 0;
};

#endif /* ABSTRACTGLFRAME_H_ */
