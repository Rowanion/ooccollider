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
