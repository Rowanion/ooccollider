/**
 * @file	RsAbstractRenderer.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 14.05.2010
 * @brief 	RsAbstractRenderer class declaration.
 */

#ifndef RSABSTRACTRENDERER_H_
#define RSABSTRACTRENDERER_H_

#include <cstdlib>

#include "GL/glew.h"
#include "Cg/cg.h"
#include "Cg/cgGL.h"

/**
 * @class RsAbstractRenderer
 * @brief Abstract class for all implementations of a renderer.
 * This is necessary, because a renderer needs to be tied to a window, which does
 * the actual calling of the callback functions.
 */
class RsAbstractRenderer
{
public:
	RsAbstractRenderer();
	virtual ~RsAbstractRenderer();

	/**
	 * @brief
	 */
	virtual void init() = 0;

	/**
	 * @brief
	 */
	virtual void display() = 0;

	/**
	 * @brief
	 * @param _w
	 * @param _h
	 */
	virtual void reshape(int _w, int _h) = 0;

	/**
	 * @brief
	 * @param _key
	 * @param _x
	 * @param _y
	 * @param _present
	 */
	virtual void keyboard(unsigned char _key, int _x, int _y, int* _present = 0);

	/**
	 * @brief
	 * @param _key
	 * @param _x
	 * @param _y
	 * @param _present
	 */
	virtual void specialKeys(int _key, int _x, int _y, int* _present = 0);

	/**
	 * @brief
	 * @param _button
	 * @param _state
	 * @param _x
	 * @param _y
	 * @param _present
	 */
	virtual void processMouse(int _button, int _state, int _x, int _y, int* _present = 0);

	/**
	 * @brief
	 * @param _x
	 * @param _y
	 * @param _present
	 */
	virtual void processMouseActiveMotion(int _x, int _y, int* _present = 0);

	/**
	 * @brief
	 * @param _x
	 * @param _y
	 * @param _present
	 */
	virtual void processMousePassiveMotion(int _x, int _y, int* _present = 0);

	/**
	 * @brief
	 * @param _state
	 * @param _present
	 */
	virtual void processMouseEntry(int _state, int* _present = 0);

	/**
	 * @brief
	 * @param _wheel
	 * @param _direction
	 * @param _x
	 * @param _y
	 * @param _present
	 */
	virtual void processMouseWheel( int _wheel, int _direction, int _x, int _y , int* _present = 0);

	/**
	 * @brief
	 * @param _present
	 */
	virtual void animator(int* _present = 0);
	virtual void debug();
protected:
	static void cgErrorHandler(CGcontext _context, CGerror _error, void* _data);
	static void cgCompileErrorHandler(CGcontext _context, CGerror _error, void* _data);
	static CGerror cgLastError;

};

#endif /* RSABSTRACTRENDERER_H_ */
