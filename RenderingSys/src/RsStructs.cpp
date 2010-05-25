/*
 * RsStructs.h
 *
 *  Created on: 14.03.2010
 *      Author: TheAvatar
 */

#include "RsStructs.h"

#include <cstring>

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

bool Tuplef::operator==(const Tuplef& _rhs) const
{
	if (this->x != _rhs.x){
		return false;
	}
	if (this->y != _rhs.y){
		return false;
	}
	return true;

}

bool Tuplef::operator<(const Tuplef& _rhs) const
{
	if (this->x == _rhs.x){
		return this->y < _rhs.y;
	}
	return this->x < _rhs.x;
}

Triplef::Triplef()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

bool Triplef::operator<(const Triplef& _rhs) const
{
	if (this->x == _rhs.x){
		if (this->y == _rhs.y){
			return this->z < _rhs.z;
		}
		return this->y < _rhs.y;
	}
	return this->x < _rhs.x;
}

bool Triplef::operator==(const Triplef& _rhs) const
{
	if (this->x != _rhs.x){
		return false;
	}
	if (this->y != _rhs.y){
		return false;
	}
	if (this->z != _rhs.z){
		return false;
	}
	return true;
}

Tripleuc::Tripleuc()
{
	x = 0;
	y = 0;
	z = 0;
}

Triplec::Triplec()
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

Quadruplec::Quadruplec()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

bool Quadruplec::operator==(const Quadruplec& _rhs) const
{
	if (this->x != _rhs.x){
		return false;
	}
	if (this->y != _rhs.y){
		return false;
	}
	if (this->z != _rhs.z){
		return false;
	}
	if (this->w != _rhs.w){
		return false;
	}
	return true;
}

bool Quadruplec::operator<(const Quadruplec& _rhs) const
{
	if (this->x == _rhs.x){
		if (this->y == _rhs.y){
			if (this->z == _rhs.z){
				return this->w < _rhs.w;
			}
			return this->z < _rhs.z;
		}
		return this->y < _rhs.y;
	}
	return this->x < _rhs.x;
}

ObjInfo::ObjInfo()
{
	groupCount = 0;
	vertexCount = 0;
	texCount = 0;
	faceCount = 0;
	normalCount = 0;
	materialCount = 0;
	groupBits = std::vector<unsigned>();
	groupFaces = std::vector<unsigned>();
}

RsV3N4T2::RsV3N4T2(){};

RsV3N4T2::RsV3N4T2(const float* _v, const char* _n, const float* _t)
{
	memcpy(&this->v, _v, sizeof(float)*3);
	memcpy(&this->n, _n, sizeof(char)*3);
	this->n.w = 1;
	memcpy(&this->t, _t, sizeof(float)*2);
}

bool RsV3N4T2::operator<(const RsV3N4T2& _rhs) const
{
	if(this->v == _rhs.v){
		if (this->n == _rhs.n){
			return this->t < _rhs.t;
		}
		return this->n < _rhs.n;
	}
	return this->v < _rhs.v;
}

