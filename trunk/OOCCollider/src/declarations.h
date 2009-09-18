/**
 * @file	declarations.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 04.05.2009
 * @brief 	declarations class declaration.
 */


#ifndef DECLARATIONS_H_
#define DECLARATIONS_H_

class AbstractEventHandler;
class AbstractGlFrame;
class DataCore;
class DataCoreGlFrame;
class Message;
class MpiControl;
class RenderCore;
class RenderCoreGlFrame;
class SimpleGlFrame;

class TestEventHandler;
class OOCWindow;

struct Tile{
	int xPos;
	int yPos;
	int width;
	int height;
	double renderTime;
};

#endif /* DECLARATIONS_H_ */
