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
#define CAMERA_OFFSET 15.0f
#define FAR_CLIPPING_PLANE 100.0f
#define EXTENDED_FOVY 70.0
#define MAX_TREE_ITERATIONS 3000
#define BASE_FOVY 70.0

#ifdef OFFICE
#define MAX_OFFLINE_VBOS 4000
#define MAX_LOADS_PER_FRAME 1923
#define BASE_MODEL_PATH "/home/ava/Diplom/Model/SampleTree"
#endif
#ifdef HOME
#define MAX_OFFLINE_VBOS 1500
#define MAX_LOADS_PER_FRAME 40
#define BASE_MODEL_PATH "/home/ava/Diplom/Model/SampleTree"
#endif
#ifdef CLUSTER
#endif

// class declarations
class AbstractEventHandler;
class AbstractGlFrame;
class DataCore;
class DataCoreGlFrame;
class OOCCamera;
class OOCWindow;
class RenderCore;
class RenderCoreGlFrame;
class RenderMasterCore;
class SimpleGlFrame;
class Splittree;

//events
class AccumulatedRendertimeEvent;
class CameraMovedEvent;
class ChangeTileDimensionsEvent;
class ColorBufferEvent;
class DepthBufferEvent;
class DepthBufferRequestEvent;
class EndTransmissionEvent;
class EndOfFrameEvent;
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
struct Tile;

#endif /* DECLARATIONS_H_ */
