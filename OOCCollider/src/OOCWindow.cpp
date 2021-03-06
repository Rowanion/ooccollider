/**
 *  @file OOCWindow.cpp
 *
 *  @date Created on: 03.05.2009
 *  @author weltmarktfuehrer@googlemail.com
 *  @brief OOCWindow class definition.
 */

#include "OOCWindow.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>

#include "error.h"
#include "AbstractGlFrame.h"
#include "KeyPressedEvent.h"
#include "MouseWheelEvent.h"
#include "WindowClosedEvent.h"
#include "MouseButtonEvent.h"
#include "KeyReleasedEvent.h"

using namespace std;

OOCWindow* OOCWindow::instance = 0;
int OOCWindow::mouseLeftState = 0;
int OOCWindow::mouseRightState = 0;

OOCWindow::OOCWindow(size_t width, size_t height, size_t color_depth,
		bool full_screen, const char* title) :
	mWidth(0), mHeight(0), mMouseX(0), mMouseY(0), mMouseActionX(0), mMouseActionY(0), mActive(false), mHandler(0),
			mGlFrame(0){
	// Make sure no other GLWindow is alive
	if (OOCWindow::instance != 0)
		THROW(std::runtime_error, "Only one GlWindow can be active.");
	OOCWindow::instance = this;

	// Initialize the GLFW system. Check for failure.
	if (glfwInit() != GL_TRUE)
		THROW(std::runtime_error, "Could not initialize GLFW.");

	// GLFW requires the amount of R, G and B bits, so we just use
	// color_depth / 3 for those. Don't worry about the precise numbers,
	// GLFW will use the nearest mode that works.
	int bitspercolor = color_depth / 3;
	int flags = full_screen ? GLFW_FULLSCREEN : GLFW_WINDOW;

//	glfwEnable(GLFW_STICKY_KEYS);
//	glfwEnable(GLFW_KEY_REPEAT);

	// Open a window with the specified settings. Check for failure.
//	glfwOpenWindowHint(GLFW_FSAA_SAMPLES,8);
	if (glfwOpenWindow(width, height, bitspercolor, bitspercolor, bitspercolor,
			0, 0, 0, flags) != GL_TRUE)
		THROW(std::runtime_error, "Could not create window.");

	// Get the actual window size (especially for full-screen this might
	// differ from the parameters given to glfwOpenWindow)
	glfwGetWindowSize(&mWidth, &mHeight);
	mActive = true;
	// Set up the OpenGL viewport for this window.
	glViewport(0, 0, mWidth, mHeight);

	glfwSetWindowTitle(title);
	// Prevent GLFW from polling for input whenever buffers are flipped,
	// which is the default. (*I'll* tell it when to poll, dammit)
//	glfwDisable(GLFW_AUTO_POLL_EVENTS);
	// Hide the system mouse cursor.
	//  glfwDisable(GLFW_MOUSE_CURSOR);

	// Register our callback functions.
	glfwSetWindowSizeCallback(windowResizeCallback);
	glfwSetWindowCloseCallback(windowClosedCallback);

	mPriEventMan = oocframework::EventManager::getSingleton();
	mPriEventMan->addListener(this, MouseButtonStateChangeEvent::classid());
}

OOCWindow::~OOCWindow()
{
	cout << "window destructor"<< endl;
//	glfwCloseWindow();
	glfwTerminate();

	mPriEventMan->removeListener(this, MouseButtonStateChangeEvent::classid());
	OOCWindow::instance = 0;
}

void
OOCWindow::poll()
{
	glfwPollEvents();

	// If window activeness is not what it was last time we checked,
	// send out an event.
	bool active = glfwGetWindowParam(GLFW_ACTIVE) == GL_TRUE;
	if (active != mActive) {
		mActive = active;
		if (mHandler != NULL) {
			if (mActive)
				mHandler->Event(EVENT_WINDOW_ACTIVE, true);
			else
				mHandler->Event(EVENT_WINDOW_ACTIVE, false);
		}
	}
}

void
OOCWindow::flip()
{
	glfwSwapBuffers();
}

void
OOCWindow::Key_Callback(int key, int action)
{
//	cout << "Key callback" << endl;
	// If there is an event handler, tell it about this key (second
	// parameter indicates whether the key was pressed or released).
	if (action == GLFW_PRESS){
		// ----------------------------------------------
//		glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS || glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS
		// ----------------------------------------------
		KeyPressedEvent kpe = KeyPressedEvent(key, (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_RSHIFT) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_LALT) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_RALT) == GLFW_PRESS));
		oocframework::EventManager::getSingleton()->fire(kpe);
	}
	if (action == GLFW_RELEASE){
		KeyReleasedEvent kre = KeyReleasedEvent(key, (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_RSHIFT) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_LCTRL) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_RCTRL) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_LALT) == GLFW_PRESS), (glfwGetKey(GLFW_KEY_RALT) == GLFW_PRESS));
		oocframework::EventManager::getSingleton()->fire(kre);
	}
	if (OOCWindow::instance->mHandler != 0)
		OOCWindow::instance->mHandler->Event(key, action == GLFW_PRESS);
}

void
OOCWindow::Mouse_Button_Callback(int button, int action)
{
	MouseButtonEvent mbe(OOCWindow::instance->mMouseX, OOCWindow::instance->mMouseY, action, button);
	oocframework::EventManager::getSingleton()->fire(mbe);
	//	cout << "MOUSE: " << button << ", action: " << action << endl;
	if (button == 0){
		if (OOCWindow::mouseLeftState!=action){
		}

		OOCWindow::mouseLeftState= action;
	}
	else if (button == 1){
		OOCWindow::mouseRightState= action;
	}
	else if (button == 2){

	}
}

void
OOCWindow::Mouse_Pos_Callback(int x, int y)
{
	// Keep mouse within screen
	if (x < 0)
		x = 0;
	else if (x > OOCWindow::instance->mWidth)
		x = OOCWindow::instance->mWidth;
	if (y < 0)
		y = 0;
	else if (y > OOCWindow::instance->mHeight)
		y = OOCWindow::instance->mHeight;

	// Update the mouse position and tell the event handler about the
	// mouse moving.
//	cout << "MOUSE MOVED!" << endl;

	if (OOCWindow::mouseLeftState == 1 || OOCWindow::mouseRightState == 1){
		MouseDraggedEvent mde(x, y);
		oocframework::EventManager::getSingleton()->fire((mde));
	}
	OOCWindow::instance->Set_Mouse(x, y);
	if (OOCWindow::instance->mHandler != 0){
		OOCWindow::instance->mHandler->Event(EVENT_MOUSE_MOVED, true);

	}
}

void
OOCWindow::windowResizeCallback(int width, int height)
{
	cout << "resize callback" << endl;
	if (OOCWindow::instance->isGlFrameAttached()){
		OOCWindow::instance->mGlFrame->reshape(width, height);
	}
	if (OOCWindow::instance->mHandler != 0)
			OOCWindow::instance->mHandler->Event(EVENT_WINDOW_RESIZED, true);
	OOCWindow::instance->mWidth = width;
	OOCWindow::instance->mHeight = height;
}

/**
 * @brief Method is called if the windowClosingIcon is clicked.
 * If the function returns GL_TRUE, the window will be closed. If GL_FALSE is
 * returned, then not.
 */
int
OOCWindow::windowClosedCallback()
{
	WindowClosedEvent mce = WindowClosedEvent();
	oocframework::EventManager::getSingleton()->fire(mce);
//	if (Window::instance->mHandler != 0)
//		Window::instance->mHandler->Event(EVENT_WINDOW_CLOSED, true);
	return GL_FALSE;
}

void
OOCWindow::mouseWheelCallback(int pos)
{
	if (pos > OOCWindow::instance->mMouseWheelPos){
		MouseWheelEvent mwe = MouseWheelEvent(MouseWheelEvent::UP);
		oocframework::EventManager::getSingleton()->fire(mwe);
	}
	else if (pos < OOCWindow::instance->mMouseWheelPos){
		MouseWheelEvent mwe = MouseWheelEvent(MouseWheelEvent::DOWN);
		oocframework::EventManager::getSingleton()->fire(mwe);
	}
	OOCWindow::instance->mMouseWheelPos = pos;
}

void
OOCWindow::attachGlFrame(AbstractGlFrame* _glFrame)
{
	if (mGlFrame != 0){
		delete mGlFrame;
		mGlFrame = 0;
	}
	mGlFrame = _glFrame;

	// because the initial reshape happened without this frame, we have to call it
	// manually once.
	int w,h;
	glfwGetWindowSize(&w,&h);
	mGlFrame->reshape(w,h);
}

bool
OOCWindow::isGlFrameAttached() const
{
	if (mGlFrame == 0)
		return false;
	else return true;
}

void
OOCWindow::setTitle(string title) const
{
	glfwSetWindowTitle(title.c_str());
}

void OOCWindow::iconify() const
{
	glfwIconifyWindow();
}

void
OOCWindow::restore() const
{
	glfwRestoreWindow();
}

void OOCWindow::enableKeyCallback() const
{
	glfwSetKeyCallback(Key_Callback);
}

void OOCWindow::enableMouseCallback() const
{
	glfwSetMouseButtonCallback(Mouse_Button_Callback);
	glfwSetMousePosCallback(Mouse_Pos_Callback);
	glfwSetMouseWheelCallback(mouseWheelCallback);

}

void OOCWindow::resize(int width, int height)
{
	glfwSetWindowSize(width, height);
	mWidth = width;
	mHeight = height;
}

int OOCWindow::getWidth()
{
	return mWidth;
}

int OOCWindow::getHeight()
{
	return mHeight;
}

void OOCWindow::notify(oocframework::IEvent& event)
{

}
