/*
 * RsStructs.h
 *
 *  Created on: 14.03.2010
 *      Author: TheAvatar
 */

#include "RsStructs.h"

RsTGAimage::RsTGAimage()
{
	data = 0;
}

RsTGAimage::~RsTGAimage()
{
	delete[] data;
	data = 0;
}

Tuplef::Tuplef()
{
	x = 0.0f;
	y = 0.0f;
}

Triplef::Triplef()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

Tripleuc::Tripleuc()
{
	x = 0;
	y = 0;
	z = 0;
}

Quadruplef::Quadruplef()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

Quadrupleuc::Quadrupleuc()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

