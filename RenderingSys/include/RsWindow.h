/*
 * RsWindow.h
 *
 *  Created on: 12.03.2010
 *      Author: TheAvatar
 */

#ifndef RSWINDOW_H_
#define RSWINDOW_H_

#include <string>

#include "RsAbstractRenderer.h"

class RsWindow {
public:
	static RsWindow* getSingleton();
	void attachRenderer(RsAbstractRenderer* _renderer);
	void detachRenderer();
	void start();
	void setWindowTitle(const char* _title);
	// static wrapper functions for glut
	static void reshape ( int _w, int _h );
	static void display();
	static void close();
	static void keyboard(unsigned char _key, int _x, int _y);
	static void specialKeys(int _key, int _x, int _y);
	static void processMouse(int _button, int _state, int _x, int _y);
	static void processMouseActiveMotion(int _x, int _y);
	static void processMousePassiveMotion(int _x, int _y);
	static void processMouseEntry(int _state);
	static void processMouseWheel( int _wheel, int _direction, int _x, int _y);
	static void animator(int _value);

	void exit();
	RsAbstractRenderer* getRenderer() const;

private:
    static RsWindow* instance;
	RsWindow();
	virtual ~RsWindow();
    class Guard {
    public:
		~Guard();
    };
    friend class Guard;

	void init();

	RsAbstractRenderer* mPriRenderer;
    int mPriWindowWidth;
    int mPriWindowHeight;
    bool mPriGlutInited;
    bool mPriWindowClosing;
    int mPriWindow;
    std::string mPriWindowTitle;
};

#endif /* RSWINDOW_H_ */
