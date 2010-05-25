/**
 * @file	Vbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	Vbo class declaration.
 */

#ifndef VBO_H_
#define VBO_H_

#include <inttypes.h>

class Vbo {
public:
	Vbo();
	virtual ~Vbo();
	virtual void draw() = 0;
	virtual unsigned getIndexCount() const = 0;
	virtual unsigned getVertexCount() const = 0;
	virtual uint64_t getComponentBytes() const = 0;
	virtual void debug();
protected:
private:

};

#endif /* VBO_H_ */
