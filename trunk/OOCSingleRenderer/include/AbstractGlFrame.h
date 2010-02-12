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

	/**
	 * @brief this function initializes the constants for frustum calculations.
	 */
	virtual void initTiles();

protected:
	virtual void resizeFrustum();
	virtual void resizeFrustum(unsigned _width, unsigned _height);

	/**
	 * @brief As the name suggests, this resizes the frustum to the given tile-position and dimension.
	 */
	virtual void resizeFrustum(unsigned tileXPos, unsigned tileYPos, unsigned tileWidth, unsigned tileHeight, float _farClipPlane);

	/**
	 * @brief This also resizes the frustum but this time to the extended size.
	 */
	virtual void resizeFrustumExt(unsigned tileXPos, unsigned tileYPos, unsigned tileWidth, unsigned tileHeight, float _farClipPlane);

	/**
	 * mProWindowHeight / mProWindowWidth;
	 */
	float mProScreenRatio;

	/**
	 * the actual window dimensions
	 */
	unsigned int mProWindowWidth, mProWindowHeight;

	/**
	 * the targeted window-dimensions, ie. the dimensions of the master-window.
	 */
	unsigned int mProTargetWindowWidth, mProTargetWindowHeight;
	float mProNearClippingPlane, mProFarClippingPlane;

	/**
	 * one unit in frustum-space. Not pixels. Used for the tangens-functions.
	 */
	double mProFrustumUnit;

	/**
	 * how far the frustum stretches to the left and to the right in frustum-space.
	 */
	double mProFrustumDistMaxX,mProFrustumDistMinX;

	/**
	 * how far the frustum stretches to the bottom and to the top in frustum-space.
	 */
	double mProFrustumDistMaxY,mProFrustumDistMinY;

	/**
	 * how far the extended frustum stretches to the left and to the right in frustum-space.
	 */
	double mProFrustumExtDistMaxX,mProFrustumExtDistMinX;

	/**
	 * how far the extended frustum stretches to the bottom and to the top in frustum-space.
	 */
	double mProFrustumExtDistMaxY,mProFrustumExtDistMinY;

	/**
	 * the amount of the frustum extension on either left and right side in pixels and in frustum-space.
	 */
	double mProFrustumExtensionX_px, mProFrustumExtensionX_size;

	/**
	 * the amount of the frustum extension on either bottom and top side in pixels and in frustum-space.
	 */
	double mProFrustumExtensionY_px, mProFrustumExtensionY_size;

	/**
	 * the final values of the frustum-borders which will then be passed to glFrustum().
	 */
	double mProWorldLeftLine, mProWorldRightLine, mProWorldTopLine, mProWorldBottomLine;


};
#endif /* ABSTRACTGLFRAME_H_ */
