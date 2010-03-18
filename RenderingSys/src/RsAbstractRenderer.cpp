/*
 * RsAbstractRenderer.cpp
 *
 *  Created on: 12.03.2010
 *      Author: TheAvatar
 */

#include "RsAbstractRenderer.h"

#include <iostream>

RsAbstractRenderer::RsAbstractRenderer()
{
	// TODO Auto-generated constructor stub

}

RsAbstractRenderer::~RsAbstractRenderer()
{
	// TODO Auto-generated destructor stub
}

void RsAbstractRenderer::keyboard(unsigned char _key, int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::specialKeys(int _key, int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::processMouse(int _button, int _state, int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::processMouseActiveMotion(int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::processMousePassiveMotion(int _x, int _y, int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::processMouseEntry(int _state, int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::processMouseWheel( int _wheel, int _direction, int _x, int _y , int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::animator(int* _present)
{
	if (_present != 0){
		*_present = -1;
		return;
	}
}

void RsAbstractRenderer::debug()
{
	std::cerr << "in AbstractClass" << std::endl;
}
