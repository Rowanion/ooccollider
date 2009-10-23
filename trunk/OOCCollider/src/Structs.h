/**
  *  @file Structs.h
  *
  *  @date Created on: 03.05.2009
  *  @author weltmarktfuehrer@googlemail.com
  */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#include <stdexcept>

struct Tile{
	Tile();
	Tile(int _xPos, int _yPos, int _width, int _height, double _renderTime);
	int xPos;
	int yPos;
	int width;
	int height;
	double renderTime;
};

#endif /* STRUCTS_H_ */
