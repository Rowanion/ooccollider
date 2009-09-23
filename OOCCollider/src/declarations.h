/**
 * @file	declarations.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	declarations class declaration.
 */


#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

// project-wide defines
#define GET_GLERROR(ret) \
{ \
         GLenum err = glGetError(); \
         if (err != GL_NO_ERROR) { \
                 fprintf(stderr, "[%s line %d] GL Error: %s\n", \
                 __FILE__, __LINE__, gluErrorString(err)); \
                 fflush(stderr); \
         } \
}
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define DEPTHBUFFER_INTERVAL 15
#define BASE_MODEL_PATH "/home/ava/Diplom/Model/Octree"

#ifdef OFFICE
#define MAX_OFFLINE_VBOS 4000
#define MAX_LOADS_PER_FRAME 80
#endif
#ifdef HOME
#define MAX_OFFLINE_VBOS 1500
#define MAX_LOADS_PER_FRAME 40
#endif
#ifdef CLUSTER
#endif

// class declarations
class AbstractEventHandler;
class AbstractGlFrame;
class DataCore;
class DataCoreGlFrame;
class Message;
class MpiControl;
class OOCWindow;
class RenderCore;
class RenderCoreGlFrame;
class RenderMasterCore;
class SimpleGlFrame;
class Splittree;

//events
class AccumulatedRendertimeEvent;
class ChangeTileDimensionsEvent;
class ColorBufferEvent;
class DepthBufferEvent;
class DepthBufferRequestEvent;
class EndTransmissionEvent;
class InfoRequestEvent;
class KeyPressedEvent;
class KillApplicationEvent;
class ModelViewMatrixEvent;
class MouseButtonEvent;
class MouseButtonStateChangeEvent;
class MouseDraggedEvent;
class MouseMovedEvent;
class MouseWheelEvent;
class NodeRequestEvent;
class VboEvent;
class WindowClosedEvent;
class WindowResizedEvent;

// struct declarations
struct Tile{
	int xPos;
	int yPos;
	int width;
	int height;
	double renderTime;
};

#endif /* DECLARATIONS_H_ */
