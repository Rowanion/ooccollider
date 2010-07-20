/**
 * @file	RsStructs.cpp
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 28.09.2009
 * @brief 	File holds definitions for different structs of the project.
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

RsTuplef::RsTuplef()
{
	x = 0.0f;
	y = 0.0f;
}

bool RsTuplef::operator==(const RsTuplef& _rhs) const
{
	if (this->x != _rhs.x){
		return false;
	}
	if (this->y != _rhs.y){
		return false;
	}
	return true;

}

bool RsTuplef::operator<(const RsTuplef& _rhs) const
{
	if (this->x == _rhs.x){
		return this->y < _rhs.y;
	}
	return this->x < _rhs.x;
}

RsTriplef::RsTriplef()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
}

RsTriplef::RsTriplef(float _x, float _y, float _z)
{
	x = _x;
	y = _y;
	z = _z;
}

bool RsTriplef::operator<(const RsTriplef& _rhs) const
{
	if (this->x == _rhs.x){
		if (this->y == _rhs.y){
			return this->z < _rhs.z;
		}
		return this->y < _rhs.y;
	}
	return this->x < _rhs.x;
}

bool RsTriplef::operator==(const RsTriplef& _rhs) const
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

RsTripleuc::RsTripleuc()
{
	x = 0;
	y = 0;
	z = 0;
}

RsTriplec::RsTriplec()
{
	x = 0;
	y = 0;
	z = 0;
}

RsQuadruplef::RsQuadruplef()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	w = 0.0f;
}

RsQuadrupleuc::RsQuadrupleuc()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

RsQuadruplec::RsQuadruplec()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

bool RsQuadruplec::operator==(const RsQuadruplec& _rhs) const
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

bool RsQuadruplec::operator<(const RsQuadruplec& _rhs) const
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

RsObjInfo::RsObjInfo()
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

RsV3N4T2::RsV3N4T2(const float* _v, const float* _t, const char* _n)
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

RsV3N4::RsV3N4(){};

RsV3N4::RsV3N4(const float* _v, const char* _n)
{
	memcpy(&this->v, _v, sizeof(float)*3);
	memcpy(&this->n, _n, sizeof(char)*3);
	this->n.w = 1;
}

bool RsV3N4::operator<(const RsV3N4& _rhs) const
{
	if(this->v == _rhs.v){
		return this->n < _rhs.n;
	}
	return this->v < _rhs.v;
}

RsV4T2::RsV4T2(){}
RsV4T2::RsV4T2(const float* _v, const char* _t)
{
	memcpy(&this->v, _v, sizeof(float)*3);
	this->v.w = 1.0f;
	memcpy(&this->t, _t, sizeof(float)*2);
}

RsV4T2::RsV4T2(float _vx, float _vy, float _vz, float _tx, float _ty)
{
	this->v.x = _vx;
	this->v.y = _vy;
	this->v.z = _vz;
	this->v.w = 1.0f;

	this->t.x = _tx;
	this->t.y = _ty;
}

RsMaterial::RsMaterial()
{
	this->id = 0;
	this->shininess = 0.0f;
	this->ambient = RsTriplef(0.2f, 0.2f, 0.2f);
	this->diffuse = RsTriplef(0.8f, 0.8f, 0.8f);
	this->specular = RsTriplef(1.0f, 1.0f, 1.0f);
	this->texture = 0;
	this->name = std::string("mat");


}

bool RsMaterial::operator<(const RsMaterial& _rhs) const
{
	return (this->id < _rhs.id);
}
