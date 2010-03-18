/*
 * RsAbstractRenderer.h
 *
 *  Created on: 12.03.2010
 *      Author: TheAvatar
 */

#ifndef RSABSTRACTRENDERER_H_
#define RSABSTRACTRENDERER_H_

class RsAbstractRenderer
{
public:
	RsAbstractRenderer();
	virtual ~RsAbstractRenderer();
	virtual void init() = 0;
	virtual void display() = 0;
	virtual void reshape(int _w, int _h) = 0;
	virtual void exit() = 0;
	virtual void keyboard(unsigned char _key, int _x, int _y, int* _present = 0);
	virtual void specialKeys(int _key, int _x, int _y, int* _present = 0);
	virtual void processMouse(int _button, int _state, int _x, int _y, int* _present = 0);
	virtual void processMouseActiveMotion(int _x, int _y, int* _present = 0);
	virtual void processMousePassiveMotion(int _x, int _y, int* _present = 0);
	virtual void processMouseEntry(int _state, int* _present = 0);
	virtual void processMouseWheel( int _wheel, int _direction, int _x, int _y , int* _present = 0);
	virtual void animator(int* _present = 0);
	virtual void debug();
};

#endif /* RSABSTRACTRENDERER_H_ */
