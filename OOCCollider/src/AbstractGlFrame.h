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
	AbstractGlFrame(int winWidth, int winHeight, int targetWinWidth, int targetWinHeight);
	virtual ~AbstractGlFrame(){};
	virtual void init() = 0;
	virtual void display() = 0;
	virtual void reshape(int width, int height) = 0;
	virtual void initTiles();

protected:
	virtual void resizeFrustum();
	virtual void resizeFrustum(unsigned _width, unsigned _height);
	virtual void resizeFrustum(unsigned tileXPos, unsigned tileYPos, unsigned tileWidth, unsigned tileHeight);
	virtual void resizeFrustumExt(unsigned tileXPos, unsigned tileYPos, unsigned tileWidth, unsigned tileHeight);

	float mProScreenRatio;
	unsigned int mProWindowWidth, mProWindowHeight;
	unsigned int mProTargetWindowWidth, mProTargetWindowHeight;
	float mProNearClippingPlane, mProFarClippingPlane;
	double mProFrustumUnit;
	double mProFrustumExtDistMaxX,mProFrustumExtDistMinX;
	double mProFrustumExtDistMaxY,mProFrustumExtDistMinY;
	double mProFrustumDistMaxX,mProFrustumDistMinX;
	double mProFrustumDistMaxY,mProFrustumDistMinY;
	double mProFrustumExtensionX_px, mProFrustumExtensionX_size;
	double mProFrustumExtensionY_px, mProFrustumExtensionY_size;
	double mProWorldLeftLine, mProWorldRightLine, mProWorldTopLine, mProWorldBottomLine;


};
#endif /* ABSTRACTGLFRAME_H_ */
