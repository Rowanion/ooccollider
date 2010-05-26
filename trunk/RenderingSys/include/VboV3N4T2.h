/**
 * @file	VboV3N4T2.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	VboV3N4T2 class declaration.
 */

#ifndef VBOV3N4T2_H_
#define VBOV3N4T2_H_

#include "Vbo.h"
#include "RsStructs.h"

#include <GL/glew.h>

#include <map>

class VboV3N4T2 : public Vbo{
public:
	VboV3N4T2();
	VboV3N4T2(unsigned _indexCount, unsigned* _indices, std::map<RsV3N4T2, unsigned>* _data);
	virtual ~VboV3N4T2();
	virtual void draw();
	virtual unsigned getIndexCount() const;
	virtual unsigned getVertexCount() const;
	virtual uint64_t getComponentBytes() const;
	virtual void debug();
protected:
	RsV3N4T2* mProData;
	unsigned mProDataCount;
	unsigned* mProIndices;
	unsigned mProIndexCount;
private:
	GLuint mPriDataId;
	GLuint mPriIndexId;

};

#endif /* VBOV3N4T2_H_ */
