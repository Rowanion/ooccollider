/**
 * @file	RsAbstractVbo.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 20.05.2010
 * @brief 	RsAbstractVbo class declaration.
 */

#ifndef RSABSTRACTVBO_H_
#define RSABSTRACTVBO_H_

#include <inttypes.h>

/**
 * @class RsAbstractVbo
 * @brief Abstract base-class for all VBOs. The idea is, to free a
 * renderer from the burden of having to know, what kind of VBO this is.
 */
class RsAbstractVbo {
public:
	RsAbstractVbo();
	virtual ~RsAbstractVbo();
	virtual void draw() = 0;
	virtual unsigned getIndexCount() const = 0;
	virtual unsigned getVertexCount() const = 0;
	virtual uint64_t getComponentBytes() const = 0;
	virtual void debug();
protected:
private:

};

#endif /* RSABSTRACTVBO_H_ */
