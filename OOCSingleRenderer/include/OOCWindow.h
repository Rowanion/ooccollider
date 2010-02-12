/**
 *  @file OOCWindow.h
 *
 *  @date Created on: 03.05.2009
 *  @author weltmarktfuehrer@googlemail.com
 *  @brief OOCWindow class declaration.
 */

#ifndef OOCWINDOW_H_
#define OOCWINDOW_H_

#include <cstddef>
#include <string>

#include "declarations.h"
#include "AbstractEventHandler.h"
#include "EventManager.h"
#include "MouseDraggedEvent.h"

#include "MouseButtonEvent.h"
#include "AbstractEventListener.h"
#include "IEvent.h"

/**
 * @class GlWindow
 * @brief This is a window-implementation based on glfw.
 *
 */
class OOCWindow: public oocframework::AbstractEventListener{
public:
	OOCWindow(size_t width, size_t height, size_t color_depth, bool full_screen,
			const char* title);
	virtual ~OOCWindow();

	/**
	 * @brief Check for new input, will call it's current event handler if something happens.
	 */
	void poll();

	/**
	 * @brief Swap the video buffers.
	 */
	void flip();

	size_t Width() const {
		return mWidth;
	}
	size_t Height() const {
		return mHeight;
	}
	int Mouse_X() const {
		return mMouseX;
	}
	int Mouse_Y() const {
		return mMouseY;
	}

	void attachGlFrame(AbstractGlFrame* _glFrame);
	bool isGlFrameAttached() const;
	const AbstractGlFrame* getGlFrame() const {	return mGlFrame;};
	void setTitle(std::string title) const;
	void iconify() const;
	void restore() const;
	void enableKeyCallback() const;
	void enableMouseCallback() const;
	int getWidth();
	int getHeight();
	void resize(int width, int height);
	virtual void notify(oocframework::IEvent& event);

private:
	int mWidth, mHeight, mMouseX, mMouseY, mMouseWheelPos, mMouseActionX, mMouseActionY;
	bool mActive;
	AbstractEventHandler* mHandler;
	AbstractGlFrame* mGlFrame;
	oocframework::EventManager* mPriEventMan;

	static int mouseLeftState;
	static int mouseRightState;


	// Used only by Mouse_Pos_Callback
	void Set_Mouse(int x, int y) {
		mMouseX = x;
		mMouseY = y;
	}

	/**
	 * @brief The instance variable is used by the callback functions
	 * to get hold of the window object.
	 * The callbacks are static, GLFW is a C library and thus requires a
	 * plain old function for callbacks.
	 */
	static OOCWindow* instance;
	static void Key_Callback(int key, int action);
	static void Mouse_Button_Callback(int button, int action);
	static void Mouse_Pos_Callback(int x, int y);
	static void windowResizeCallback(int width, int height);
	static int windowClosedCallback();
	static void mouseWheelCallback(int pos);
};

#endif /* OOCWINDOW_H_ */
