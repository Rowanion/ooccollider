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
#undef SHOW_RENDER_IMAGE
#undef SHOW_DEPTH_IMAGE
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define TARGET_WIDTH 640
#define TARGET_HEIGHT 480
#define DEPTHBUFFER_INTERVAL 16
#define CAMERA_OFFSET 15.0f
#define FAR_CLIPPING_PLANE 100.0f
#define MAX_TREE_ITERATIONS 3000
#define BASE_FOVY 70.0
#define PRESELECTED_SEED 670274678
#define MODULO_FRAMECOUNT 5000
#define CCOLLISION_AVG 50
#define DISPLAY_AVG 50
#define OCULLING_AVG 50
#define REQUESTDATA_AVG 50
#define FRUSTUMCULLING_AVG 50
#define DEPTH_AVG 10
#define DISTANCE_RENEWAL 10
#define LVL_OF_REDUNDANCY 1
#define MODEL_DIR "/SampleTree_packed"

#undef DEBUG_CCOLLISION
#undef DEBUG_RENDERLOOP
#undef DEBUG_FRUSTUMCULLING
#undef DEBUG_OCULLING
#undef DEBUG_REQUESTDATA
#undef DEBUG_INITIAL_SEND
#define DEBUG_DEPTH_RESEND
#define KEEP_VBOS_RESIDENT

#ifdef OFFICE
#define SHOW_RENDER_IMAGE
#undef SHOW_DEPTH_IMAGE
#define MAX_OFFLINE_VBOS 4000
#define MAX_LOADS_PER_FRAME 1000
#define BASE_MODEL_PATH "/home/ava/Diplom/Model"
#define L1_CACHE_THRESHOLD 412000000
#define L1_CACHE_VBO_LIMIT 20000
//#define L2_CACHE_THRESHOLD 1073741824
#define L2_CACHE_THRESHOLD 736870912
#endif

#ifdef PC2
#define KEEP_VBOS_RESIDENT
#undef SHOW_RENDER_IMAGE
#undef SHOW_DEPTH_IMAGE
#define KEEP_VBOS_RESIDENT
#define MAX_OFFLINE_VBOS 4000
#define MAX_LOADS_PER_FRAME 1000
//#define BASE_MODEL_PATH "/home/ava/Diplom/Model/SampleTree"
#define BASE_MODEL_PATH "/home-pc2/user/ava/Diplom/Model"
#define L1_CACHE_THRESHOLD 412000000
#define L1_CACHE_VBO_LIMIT 10000
//#define L2_CACHE_THRESHOLD 1073741824
#define L2_CACHE_THRESHOLD 736870912
#endif

#ifdef HOME
#undef KEEP_VBOS_RESIDENT
#define SHOW_RENDER_IMAGE
#define SHOW_DEPTH_IMAGE
#define MAX_OFFLINE_VBOS 1500
#define MAX_LOADS_PER_FRAME 40
#define BASE_MODEL_PATH "/home/ava/Diplom/Model"
#define L1_CACHE_THRESHOLD 153600
#define L1_CACHE_VBO_LIMIT 3000
#define L2_CACHE_THRESHOLD 268435456
#endif

// class declarations
class AbstractEventHandler;
class AbstractGlFrame;
class DataCore;
class DataCoreGlFrame;
class MemTools;
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
class ClearCacheEvent;
class ColorBufferEvent;
class DepthBufferEvent;
class DepthBufferRequestEvent;
class EndOfFrameEvent;
class EndTransmissionEvent;
class InfoRequestEvent;
class JobDoneEvent;
class KeyPressedEvent;
class KillApplicationEvent;
class ModelViewMatrixEvent;
class MouseButtonEvent;
class MouseButtonStateChangeEvent;
class MouseDraggedEvent;
class MouseMovedEvent;
class MouseWheelEvent;
class NodeRequestEvent;
class OcclusionRequestEvent;
class RandomSeedEvent;
class VboEvent;
class WindowClosedEvent;
class WindowResizedEvent;

// struct declarations
struct Tile;

#endif /* DECLARATIONS_H_ */
