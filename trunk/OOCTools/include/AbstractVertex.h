/**
 * @file	AbstractVertex.h
 * @author  TheAvatar <weltmarktfuehrer@googlemail.com>
 * @version 1.0
 * @date	Created on: 02.02.2009
 * @brief 	AbstractVertex class declaration.
 */

#ifndef ABSTRACTVERTEX_H_
#define ABSTRACTVERTEX_H_

namespace ooctools {

/**
 * @class AbstractVertex
 *
 * @brief Generic vertex class.
 * @deprecated This is just experimental and is unused.
 *
 *
 * This class should represent a generic template for vertices. It dosen't work, so
 * it's discontinued.
 */
template <typename T>
class AbstractVertex {
public:
	AbstractVertex();
	AbstractVertex(const AbstractVertex<T>& src);
	AbstractVertex(const T filler);
	virtual ~AbstractVertex();
	virtual int getCount();
	virtual const T* getData() const {return (const T*) mPriData;};


	virtual void setCount(int c);

protected:
	T mPriData[];
private:
	int _count; // for Texture-Coords which may have only x/y
	int mPriCount;
};

} // ooctools

#endif /* ABSTRACTVERTEX_H_ */
