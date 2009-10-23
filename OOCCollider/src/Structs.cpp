/**
  *  @file Structs.cpp
  *
  *  @date Created on: 03.05.2009
  *  @author weltmarktfuehrer@googlemail.com
  */


#include "Structs.h"

Tile::Tile()
{
}

Tile::Tile(int _xPos, int _yPos, int _width, int _height, double _renderTime) :
		xPos(_xPos), yPos(_yPos), width(_width), height(_height), renderTime(_renderTime)

{

}
