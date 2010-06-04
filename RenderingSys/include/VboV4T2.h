/**
 * @file	VboV4T2.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV4T2 class declaration.
 */

#ifndef VBOV4T2_H_
#define VBOV4T2_H_

#include "Vbo.h"
#include "RsStructs.h"
#include <map>

#include <GL/glew.h>

class VboV4T2 : public Vbo{
public:
	VboV4T2(unsigned _indexCount, const unsigned* _indices, const std::map<RsV4T2, unsigned>* _data);
	VboV4T2(unsigned _indexCount, const unsigned* _indices, unsigned _dataCount, const RsV4T2* _data, GLenum _type);
	virtual ~VboV4T2();
	virtual void draw();
	virtual unsigned getIndexCount() const;
	virtual unsigned getVertexCount() const;
	virtual uint64_t getComponentBytes() const;
protected:
	RsV4T2* mProData;
	unsigned mProDataCount;
	unsigned* mProIndices;
	unsigned mProIndexCount;
private:
	GLenum mPriFaceType;
	GLuint mPriDataId;
	GLuint mPriIndexId;
};

#endif /* VBOV4T2_H_ */
