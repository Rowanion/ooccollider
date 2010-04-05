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
	data[0] = 0.0f;
	data[1] = 0.0f;
}

Triplef::Triplef()
{
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
}

Tripleuc::Tripleuc()
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
}

Quadruplef::Quadruplef()
{
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;
}

Quadrupleuc::Quadrupleuc()
{
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
}

