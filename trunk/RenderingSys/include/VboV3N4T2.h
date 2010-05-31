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
#include "RsMathTools.h"

#include <GL/glew.h>

#include <map>

class VboV3N4T2 : public Vbo{
public:

	/**
	 * @brief This constructs a new VBO of this type by the given parameters.
	 * @param _indexCount how many indices do we have?
	 * @param _indices a const pointer to our index data.
	 * @param _data a const pointer to a std::map containing each vertex with its index.
	 */
	VboV3N4T2(unsigned _indexCount, const unsigned* _indices, const std::map<RsV3N4T2, unsigned>* _data);
	virtual ~VboV3N4T2();

	/**
	 * @brief Simply calls the relevant OpenGL drawing commands.
	 */
	virtual void draw();

	/**
	 * @brief Simple getter method for number of indices contained within this VBO.
	 * @return the number indices
	 */
	virtual unsigned getIndexCount() const;

	/**
	 * @brief Simple getter method for number of vertices contained within this VBO.
	 * @return the number of vertices
	 */
	virtual unsigned getVertexCount() const;

	/**
	 * @brief This a virtualized sizeof()-method  for a single vertex-component.
	 * @return the number of bytes a single vertex of this class has
	 */
	virtual uint64_t getComponentBytes() const;
	virtual void debug();
protected:
	RsV3N4T2T3* mProData;
	unsigned mProDataCount;
	unsigned* mProIndices;
	unsigned mProIndexCount;
private:
	GLuint mPriDataId;
	GLuint mPriIndexId;
	RsMathTools mPriMath;

};

#endif /* VBOV3N4T2_H_ */
