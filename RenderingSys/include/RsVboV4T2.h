/**
 * @file	RsVboV4T2.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsVboV4T2 class declaration.
 */

#ifndef RSVBOV4T2_H_
#define RSVBOV4T2_H_

#include "RsAbstractVbo.h"
#include "RsStructs.h"
#include <map>

#include <GL/glew.h>

/**
 * @class RsVboV4T2
 * @brief A VBO-class for 4-component float vertices and 2-component texture-coordinates.
 */
class RsVboV4T2 : public RsAbstractVbo{
public:
	RsVboV4T2(unsigned _indexCount, const unsigned* _indices, const std::map<RsV4T2, unsigned>* _data);
	RsVboV4T2(unsigned _indexCount, const unsigned* _indices, unsigned _dataCount, const RsV4T2* _data, GLenum _type);
	virtual ~RsVboV4T2();
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

#endif /* RSVBOV4T2_H_ */
