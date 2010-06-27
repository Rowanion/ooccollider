/**
 * @file	RsVboV4.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsVboV4 class declaration.
 */

#ifndef RSVBOV4_H_
#define RSVBOV4_H_

#include "RsAbstractVbo.h"
#include "RsStructs.h"

/**
 * @class RsVboV4
 * @brief A VBO-class for 4-component float vertices.
 */
class RsVboV4 : public RsAbstractVbo{
public:
	RsVboV4();
	RsVboV4(unsigned _faceCount, unsigned _vertexCount);
	virtual ~RsVboV4();
	virtual void draw();
	virtual unsigned getIndexCount() const;
	virtual unsigned getVertexCount() const;
	virtual uint64_t getComponentBytes() const;
protected:
	RsV4* mProData;
	unsigned* mProFaces;
};

#endif /* RSVBOV4_H_ */
