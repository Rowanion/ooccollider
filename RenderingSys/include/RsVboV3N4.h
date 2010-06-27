/**
 * @file	RsVboV3N4.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsVboV3N4 class declaration.
 */

#ifndef RSVBOV3N4_H_
#define RSVBOV3N4_H_

#include "RsAbstractVbo.h"
#include "RsStructs.h"
#include "RsMathTools.h"

#include <map>
#include <GL/glew.h>

/**
 * @class RsVboV3N4
 * @brief A VBO-class for 3-component float vertices and 4-component normal vectors.
 */
class RsVboV3N4 : public RsAbstractVbo{
public:
	RsVboV3N4();
	RsVboV3N4(unsigned _indexCount, const unsigned* _indices, const std::map<RsV3N4, unsigned>* _data);
	virtual ~RsVboV3N4();
	virtual void draw();
	virtual unsigned getIndexCount() const;
	virtual unsigned getVertexCount() const;
	virtual uint64_t getComponentBytes() const;
protected:
	RsV3N4T3* mProData;
	unsigned mProDataCount;
	unsigned* mProIndices;
	unsigned mProIndexCount;
private:
	GLuint mPriDataId;
	GLuint mPriIndexId;
	RsMathTools mPriMath;

};

#endif /* RSVBOV3N4_H_ */
