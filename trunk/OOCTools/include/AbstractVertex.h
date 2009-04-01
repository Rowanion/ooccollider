/*
 * AbstractVertex.h
 *
 *  Created on: 02.02.2009
 *      Author: ava
 */

#ifndef ABSTRACTVERTEX_H_
#define ABSTRACTVERTEX_H_

namespace ooctools {

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
