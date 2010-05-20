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

class VboV4T2 : public Vbo{
public:
	VboV4T2();
	virtual ~VboV4T2();
protected:
	RsV4T2 mProData;
};

#endif /* VBOV4T2_H_ */
