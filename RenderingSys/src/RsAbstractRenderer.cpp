/**
 * @file	RsAbstractRenderer.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	RsAbstractRenderer class definition.
 */


#include "RsAbstractRenderer.h"

#include <iostream>

CGerror RsAbstractRenderer::cgLastError = CG_NO_ERROR;

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

void RsAbstractRenderer::cgErrorHandler(CGcontext _context, CGerror _error, void* _data)
{
	if (_data != 0){
		std::cerr << (char*)_data << ": Error: " << cgGetErrorString(_error) << std::endl;
	}
	else {
		std::cerr << "Error: " << cgGetErrorString(_error) << std::endl;
	}
}

void RsAbstractRenderer::cgCompileErrorHandler(CGcontext _context, CGerror _error, void* _data)
{
	cgLastError = _error;
	std::cerr << CG_COMPILER_ERROR << std::endl;

	if (_data != 0){
		std::cerr << (char*)_data << ": CG Error: " << cgGetErrorString(_error) << std::endl;

	}
	else {
		std::cerr << "Error (" << _error << "): " << cgGetErrorString(_error) << std::endl;
	}
	if (_context != 0) {
		std::cerr << "" << cgGetLastListing(_context) << std::endl;
	}

//	exit(0);
}
