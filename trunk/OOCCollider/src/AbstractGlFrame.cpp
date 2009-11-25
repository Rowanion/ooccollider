/**
 * @file	AbstractGlFrame.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	AbstractGlFrame class definition.
 */

#include "AbstractGlFrame.h"
#include "declarations.h"
#include "GlToolkit.h"
#include "GeometricOps.h"

#include <cmath>

AbstractGlFrame::AbstractGlFrame(int winWidth, int winHeight, int targetWinWidth, int targetWinHeight) :
	mProWindowWidth(winWidth), mProWindowHeight(winHeight), mProTargetWindowWidth(targetWinWidth),
	mProTargetWindowHeight(targetWinHeight), mProNearClippingPlane(0.1f), mProFarClippingPlane(FAR_CLIPPING_PLANE)
{
}

void AbstractGlFrame::initTiles()
{

	float fovy = BASE_FOVY;

	mProScreenRatio = (float)mProWindowHeight / (float)mProWindowWidth;

	// half width of frustum in frustum-space
	float halfFrustumLength = tan(fovy * 0.5 * ooctools::GeometricOps::PI / 180.0) * mProNearClippingPlane;
	mProFrustumUnit = halfFrustumLength / (0.5 * mProTargetWindowWidth);
	float extHalfFrustumLength = mProFrustumUnit * (0.5 * mProWindowWidth);

	mProFrustumDistMaxY = halfFrustumLength * mProScreenRatio;
	mProFrustumDistMaxX = halfFrustumLength;

	mProFrustumDistMinY = -mProFrustumDistMaxY;
	mProFrustumDistMinX = -mProFrustumDistMaxX;

	// --------------- extended -----------------------

	mProFrustumExtDistMaxY = extHalfFrustumLength * mProScreenRatio;
	mProFrustumExtDistMaxX = extHalfFrustumLength;

	mProFrustumExtDistMinY = -mProFrustumExtDistMaxY;
	mProFrustumExtDistMinX = -mProFrustumExtDistMaxX;

	mProFrustumExtensionX_px = (0.5 * mProWindowWidth) - (0.5 * mProTargetWindowWidth);
	mProFrustumExtensionX_size = mProFrustumExtensionX_px * mProFrustumUnit;
	mProFrustumExtensionY_px = (0.5 * mProWindowHeight) - (0.5 * mProTargetWindowHeight);
	mProFrustumExtensionY_size = mProFrustumExtensionY_px * mProFrustumUnit;
}

void AbstractGlFrame::resizeFrustum() {
	this->resizeFrustum(0, 0, mProWindowWidth, mProWindowHeight);
}

void AbstractGlFrame::resizeFrustum(unsigned _width, unsigned _height) {
	this->resizeFrustum(0, 0, _width, _height);
}

void AbstractGlFrame::resizeFrustum(unsigned tileXPos, unsigned tileYPos, unsigned tileWidth, unsigned tileHeight)
{
	if (tileHeight == 0)
		tileHeight = 1;
	if (tileWidth == 0)
		tileWidth = 1;

	//	float aFac = 400/320;

	mProWorldLeftLine = (double) tileXPos / (double) mProTargetWindowWidth;
	mProWorldRightLine = (double) (tileXPos + tileWidth) / (double) mProTargetWindowWidth;
	mProWorldTopLine = (double) tileYPos / (double) mProTargetWindowHeight;
	mProWorldBottomLine = (double) (tileYPos + tileHeight) / (double) mProTargetWindowHeight;

	glViewport(0, 0, (GLint) tileWidth, (GLint) tileHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//	worldTopLine = screenYMinV + ((screenYMaxV - screenYMinV) * worldTopLine);
	//	worldBottomLine = screenYMinV
	//			+ ((screenYMaxV - screenYMinV) * worldBottomLine);
	//
	//	worldLeftLine = screenYMinH + ((screenYMaxH - screenYMinH) * worldLeftLine);
	//	worldRightLine = screenYMinH + ((screenYMaxH - screenYMinH)
	//			* worldRightLine);

	mProWorldLeftLine = mProFrustumDistMinX + (tileXPos * mProFrustumUnit);
	mProWorldRightLine = mProWorldLeftLine + (tileWidth * mProFrustumUnit);
	mProWorldTopLine = mProFrustumDistMinY + (tileYPos * mProFrustumUnit);
	mProWorldBottomLine = mProWorldTopLine + (tileHeight * mProFrustumUnit);
	//	wl_strich = wl - frustumExtension_size;
	//	wr_strich = wr + frustumExtension_size;

	// how to get it to r_strich?
	//	worldRightLine = frustumDistMinX + ((frustumDistMaxX - frustumDistMinX)
	//			* worldRightLine);
	//	worldLeftLine = frustumDistMinX + ((frustumDistMaxX - frustumDistMinX) * worldLeftLine);

	//	worldTopLine = frustumDistMinY + ((frustumDistMaxY - frustumDistMinY) * worldTopLine);
	//	worldBottomLine = frustumDistMinY
	//			+ ((frustumDistMaxY - frustumDistMinY) * worldBottomLine);



	glFrustum(mProWorldLeftLine, mProWorldRightLine, mProWorldTopLine, mProWorldBottomLine,
			mProNearClippingPlane, mProFarClippingPlane);


	glMatrixMode(GL_MODELVIEW);
}

void AbstractGlFrame::resizeFrustumExt(unsigned tileXPos, unsigned tileYPos, unsigned tileWidth, unsigned tileHeight)
{
	if (tileHeight == 0)
		tileHeight = 1;
	if (tileWidth == 0)
		tileWidth = 1;

	//		float aFac = 400/320;

	mProWorldLeftLine = ((double) tileXPos / (double) mProTargetWindowWidth );
	mProWorldRightLine = ((double) (tileXPos + tileWidth) / (double) mProTargetWindowWidth );
	mProWorldTopLine = ((double) tileYPos / (double) mProTargetWindowHeight );
	mProWorldBottomLine = ((double) (tileYPos + tileHeight) / (double) mProTargetWindowHeight);

	glViewport(0, 0, (int) tileWidth+(2*mProFrustumExtensionX_px), (int) tileHeight+(2*mProFrustumExtensionY_px));

	//	if (extendFrustum){
	//		glViewport(0, 0, (GLint) 800, (GLint) 600);
	//	}
	//	else{
	//		glViewport(0, 0, (GLint) tileswidth, (GLint) tilesheight);
	//	}
	//			glViewport(0, 0, (GLint) 800, (GLint) 600);
	//		glViewport(0, 0, (GLint) 640, (GLint) 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//	worldLeftLine = frustumExtDistMinX + ((frustumExtDistMaxX - frustumExtDistMinX) * worldLeftLine);
	//	worldRightLine = frustumExtDistMinX + ((frustumExtDistMaxX - frustumExtDistMinX)
	//			* worldRightLine);
	//
	//	worldTopLine = frustumExtDistMinY + ((frustumExtDistMaxY - frustumExtDistMinY) * worldTopLine);
	//	worldBottomLine = frustumExtDistMinY
	//			+ ((frustumExtDistMaxY - frustumExtDistMinY) * worldBottomLine);

	//	tileXPos -= frustumExtensionX_px;
	//	tileYPos -= frustumExtensionY_px;
	tileWidth += mProFrustumExtensionX_px*2;
	tileHeight += mProFrustumExtensionY_px*2;
	mProWorldLeftLine = mProFrustumExtDistMinX + ((tileXPos) * mProFrustumUnit);
	mProWorldRightLine = mProWorldLeftLine + ((tileWidth) * mProFrustumUnit);
	mProWorldTopLine = mProFrustumExtDistMinY + ((tileYPos) * mProFrustumUnit);
	mProWorldBottomLine = mProWorldTopLine + ((tileHeight) * mProFrustumUnit);

	GET_GLERROR(0);
	glFrustum(mProWorldLeftLine, mProWorldRightLine, mProWorldTopLine, mProWorldBottomLine,
			mProNearClippingPlane, mProFarClippingPlane);
	GET_GLERROR(0);


	glMatrixMode(GL_MODELVIEW);

}


