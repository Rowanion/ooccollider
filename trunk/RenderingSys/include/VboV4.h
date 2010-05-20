/**
 * @file	VboV4.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV4 class declaration.
 */

#ifndef VBOV4_H_
#define VBOV4_H_

#include "Vbo.h"
#include "RsStructs.h"

class VboV4 : public Vbo{
public:
	VboV4();
	VboV4(unsigned _faceCount, unsigned _vertexCount);
	virtual ~VboV4();
	virtual void draw();
	virtual unsigned getIndexCount() const;
	virtual unsigned getVertexCount() const;
	virtual uint64_t getComponentBytes() const;
protected:
	RsV4* mProData;
	unsigned* mProFaces;
};

#endif /* VBOV4_H_ */
